#pragma once

#include <expected>
#include <string>
#include <functional>
#include <span>

namespace fugue::audio {

class IAudioEngine {
public:
    virtual ~IAudioEngine() = default;

    // The callback is called by the audio engine thread to request more samples.
    // The span is the interleaved audio buffer (e.g. 2 channels, f32).
    using RenderCallback = std::function<void(std::span<float> buffer)>;

    virtual auto init(uint32_t sample_rate, uint8_t channels, RenderCallback callback) -> std::expected<void, std::string> = 0;
    virtual auto shutdown() -> void = 0;

    virtual auto start() -> std::expected<void, std::string> = 0;
    virtual auto stop() -> void = 0;

    virtual auto set_volume(float volume) -> void = 0;
    virtual auto get_volume() const -> float = 0;
};

} // namespace fugue::audio
