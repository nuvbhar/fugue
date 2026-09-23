#pragma once

#include "audio/track.hpp"
#include <expected>
#include <string>
#include <span>

namespace fugue::audio {

struct DecoderResult {
    size_t frames_decoded{0};
    bool end_of_file{false};
};

class IDecoder {
public:
    virtual ~IDecoder() = default;

    virtual auto open(const std::filesystem::path& path) -> std::expected<TrackInfo, std::string> = 0;
    virtual auto decode(std::span<float> output_buffer) -> DecoderResult = 0;
    virtual auto seek(float position_secs) -> std::expected<void, std::string> = 0;
    virtual auto close() -> void = 0;
};

} // namespace fugue::audio
