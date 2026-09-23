#define MINIAUDIO_IMPLEMENTATION
#include "audio/miniaudio_engine.hpp"

namespace fugue::audio {

MiniaudioEngine::~MiniaudioEngine() {
    shutdown();
}

auto MiniaudioEngine::init(uint32_t sample_rate, uint8_t channels, RenderCallback callback) -> std::expected<void, std::string> {
    if (initialized_) return std::unexpected("Engine already initialized");

    callback_ = std::move(callback);

    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.format   = ma_format_f32;
    config.playback.channels = channels;
    config.sampleRate        = sample_rate;
    config.dataCallback      = data_callback;
    config.pUserData         = this;

    if (ma_device_init(nullptr, &config, &device_) != MA_SUCCESS) {
        return std::unexpected("Failed to initialize miniaudio device");
    }

    initialized_ = true;
    return {};
}

auto MiniaudioEngine::shutdown() -> void {
    if (initialized_) {
        ma_device_uninit(&device_);
        initialized_ = false;
    }
}

auto MiniaudioEngine::start() -> std::expected<void, std::string> {
    if (!initialized_) return std::unexpected("Engine not initialized");
    if (ma_device_start(&device_) != MA_SUCCESS) {
        return std::unexpected("Failed to start miniaudio device");
    }
    return {};
}

auto MiniaudioEngine::stop() -> void {
    if (initialized_) {
        ma_device_stop(&device_);
    }
}

auto MiniaudioEngine::set_volume(float volume) -> void {
    volume_ = (std::max)(0.0f, (std::min)(volume, 1.0f));
    if (initialized_) {
        ma_device_set_master_volume(&device_, volume_);
    }
}

auto MiniaudioEngine::get_volume() const -> float {
    return volume_;
}

void MiniaudioEngine::data_callback(ma_device* pDevice, void* pOutput, const void* /*pInput*/, ma_uint32 frameCount) {
    auto* engine = static_cast<MiniaudioEngine*>(pDevice->pUserData);
    if (!engine || !engine->callback_) return;

    size_t num_samples = frameCount * pDevice->playback.channels;
    std::span<float> buffer(static_cast<float*>(pOutput), num_samples);
    
    // Fill buffer with 0 first
    std::fill(buffer.begin(), buffer.end(), 0.0f);

    // Call user callback to fill audio
    engine->callback_(buffer);
}

} // namespace fugue::audio
