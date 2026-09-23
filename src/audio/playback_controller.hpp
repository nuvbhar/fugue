#pragma once

#include "audio/audio_engine.hpp"
#include "audio/decoder.hpp"
#include "audio/ring_buffer.hpp"
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace fugue::audio {

class PlaybackController {
public:
    enum class State { Stopped, Playing, Paused };

    PlaybackController(std::unique_ptr<IAudioEngine> engine, std::unique_ptr<IDecoder> decoder);
    ~PlaybackController();

    auto play(const std::filesystem::path& path) -> std::expected<void, std::string>;
    auto pause() -> void;
    auto resume() -> void;
    auto stop() -> void;
    auto seek(float position_secs) -> void;
    
    auto set_volume(float volume) -> void;
    auto get_volume() const -> float;

    auto state() const -> State;
    auto current_track() const -> std::optional<TrackInfo>;
    
    // Elapsed time in seconds
    auto position() const -> float;

private:
    auto audio_callback(std::span<float> buffer) -> void;
    auto decoder_thread_loop() -> void;

    std::unique_ptr<IAudioEngine> engine_;
    std::unique_ptr<IDecoder> decoder_;
    
    RingBuffer ring_buffer_;
    std::optional<TrackInfo> current_track_;
    
    std::atomic<State> state_{State::Stopped};
    std::atomic<float> position_{0.0f};
    
    // Threading for decoder
    std::thread decode_thread_;
    std::atomic<bool> thread_running_{true};
    std::mutex decode_mutex_;
    std::condition_variable decode_cv_;
    
    // Seek request
    std::atomic<bool> seek_requested_{false};
    std::atomic<float> seek_target_{0.0f};
};

} // namespace fugue::audio
