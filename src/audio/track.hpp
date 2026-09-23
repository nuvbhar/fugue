#pragma once

#include <filesystem>
#include <string>
#include <cstdint>

namespace fugue::audio {

struct TrackInfo {
    std::filesystem::path path;
    std::string title;
    std::string artist;
    std::string album;
    float duration_secs{0.0f};
    uint32_t sample_rate{0};
    uint8_t channels{0};
};

} // namespace fugue::audio
