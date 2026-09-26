#include "audio/playback_controller.hpp"
#include <chrono>
#include <algorithm>

using namespace std::chrono_literals;

namespace fugue::audio {

constexpr size_t RING_BUFFER_SIZE = 48000 * 2 * 4; // ~4 seconds at 48kHz stereo
constexpr size_t DECODE_CHUNK_SIZE = 48000 * 2;     // 1 second decode chunks

PlaybackController::PlaybackController(std::unique_ptr<IAudioEngine> engine, std::unique_ptr<IDecoder> decoder)
    : engine_(std::move(engine)), decoder_(std::move(decoder)), ring_buffer_(RING_BUFFER_SIZE) {
    
    decode_thread_ = std::thread([this]() { decoder_thread_loop(); });
}

PlaybackController::~PlaybackController() {
    stop();
    {
        std::lock_guard<std::mutex> lock(decode_mutex_);
        thread_running_ = false;
    }
    decode_cv_.notify_one();
    if (decode_thread_.joinable()) {
        decode_thread_.join();
    }
}

auto PlaybackController::play(const std::filesystem::path& path) -> std::expected<void, std::string> {
    stop();

    std::expected<TrackInfo, std::string> track_info;
    {
        std::lock_guard<std::mutex> lock(decode_mutex_);
        track_info = decoder_->open(path);
    }
    
    if (!track_info) {
        return std::unexpected(track_info.error());
    }

    current_track_ = *track_info;
    position_ = 0.0f;
    end_of_file_ = false;
    flush_generation_ = 0;
    audio_flush_generation_ = 0;
    ring_buffer_.clear();

    auto init_result = engine_->init(current_track_->sample_rate, current_track_->channels, 
        [this](std::span<float> buffer) { audio_callback(buffer); });
        
    if (!init_result) {
        return std::unexpected(init_result.error());
    }

    state_ = State::Playing;
    
    {
        std::lock_guard<std::mutex> lock(decode_mutex_);
        decode_cv_.notify_one();
    }

    return engine_->start();
}

auto PlaybackController::pause() -> void {
    if (state_ == State::Playing) {
        state_ = State::Paused;
    }
}

auto PlaybackController::resume() -> void {
    if (state_ == State::Paused) {
        state_ = State::Playing;
        std::lock_guard<std::mutex> lock(decode_mutex_);
        decode_cv_.notify_one();
    }
}

auto PlaybackController::stop() -> void {
    state_ = State::Stopped;
    engine_->stop();
    engine_->shutdown();
    {
        std::lock_guard<std::mutex> lock(decode_mutex_);
        decoder_->close();
        seek_requested_ = false;
    }
    current_track_.reset();
    position_ = 0.0f;
}

auto PlaybackController::seek(float position_secs) -> void {
    if (state_ != State::Stopped && current_track_) {
        std::lock_guard<std::mutex> lock(decode_mutex_);
        seek_target_ = (std::clamp)(position_secs, 0.0f, current_track_->duration_secs);
        seek_requested_ = true;
        end_of_file_ = false;
        flush_generation_.fetch_add(1, std::memory_order_relaxed);
        decode_cv_.notify_one();
    }
}

auto PlaybackController::set_volume(float volume) -> void {
    engine_->set_volume(volume);
}

auto PlaybackController::get_volume() const -> float {
    return engine_->get_volume();
}

auto PlaybackController::state() const -> State {
    return state_.load();
}

auto PlaybackController::current_track() const -> std::optional<TrackInfo> {
    return current_track_;
}

auto PlaybackController::position() const -> float {
    return position_.load();
}

auto PlaybackController::audio_callback(std::span<float> buffer) -> void {
    if (state_ != State::Playing) {
        std::fill(buffer.begin(), buffer.end(), 0.0f);
        return;
    }

    if (audio_flush_generation_.load(std::memory_order_acquire) != flush_generation_.load(std::memory_order_relaxed)) {
        ring_buffer_.clear();
        audio_flush_generation_.store(flush_generation_.load(std::memory_order_relaxed), std::memory_order_release);
    }

    size_t read = ring_buffer_.pop(buffer);
    if (read < buffer.size()) {
        std::fill(buffer.begin() + read, buffer.end(), 0.0f);
    }
    
    if (current_track_ && current_track_->sample_rate > 0) {
        float secs_played = static_cast<float>(read) / (current_track_->sample_rate * current_track_->channels);
        position_ = position_ + secs_played;
    }

    if (end_of_file_.load() && read == 0) {
        state_ = State::Stopped;
    }

    decode_cv_.notify_one(); // wake up decoder if it was waiting for space
}

auto PlaybackController::decoder_thread_loop() -> void {
    std::vector<float> decode_buf(DECODE_CHUNK_SIZE);

    while (true) {
        std::unique_lock<std::mutex> lock(decode_mutex_);
        decode_cv_.wait(lock, [this]() {
            return !thread_running_ || 
                   seek_requested_ || 
                   (state_ == State::Playing && !end_of_file_ && ring_buffer_.available_write() >= DECODE_CHUNK_SIZE);
        });

        if (!thread_running_) break;

        if (seek_requested_) {
            auto res = decoder_->seek(seek_target_);
            if (res) {
                position_ = seek_target_.load();
            }
            seek_requested_ = false;
            continue; // Re-evaluate wait conditions
        }

        if (state_ == State::Playing && !end_of_file_) {
            size_t available_write = ring_buffer_.available_write();
            if (available_write > 0) {
                size_t to_decode = (std::min)(available_write, decode_buf.size());
                std::span<float> out_span(decode_buf.data(), to_decode);
                
                auto result = decoder_->decode(out_span);
                
                if (result.frames_decoded > 0) {
                    ring_buffer_.push(std::span<const float>(decode_buf.data(), result.frames_decoded));
                }
                
                if (result.end_of_file && result.frames_decoded == 0) {
                    end_of_file_ = true;
                }
            }
        }
    }
}

} // namespace fugue::audio
