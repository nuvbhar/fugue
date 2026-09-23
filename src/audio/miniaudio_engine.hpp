#pragma once

#include "audio/audio_engine.hpp"
#include <miniaudio.h>

namespace fugue::audio {

class MiniaudioEngine : public IAudioEngine {
public:
    MiniaudioEngine() = default;
    ~MiniaudioEngine() override;

    auto init(uint32_t sample_rate, uint8_t channels, RenderCallback callback) -> std::expected<void, std::string> override;
    auto shutdown() -> void override;

    auto start() -> std::expected<void, std::string> override;
    auto stop() -> void override;

    auto set_volume(float volume) -> void override;
    auto get_volume() const -> float override;

private:
    static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

    ma_device device_{};
    bool initialized_{false};
    float volume_{1.0f};
    RenderCallback callback_;
};

} // namespace fugue::audio
