#pragma once

#include <string>

namespace fugue::terminal {

enum class ImageProtocol {
    None,       // No image support
    Braille,    // Always available (Unicode block chars)
    Sixel,      // DEC Sixel
    ITerm2,     // iTerm2 inline images
    Kitty,      // Kitty graphics protocol
};

struct TerminalCapabilities {
    bool truecolor{false};
    bool unicode{true};
    ImageProtocol image_protocol{ImageProtocol::Braille};
    std::string term_program{};
};

auto detect_capabilities() -> TerminalCapabilities;

} // namespace fugue::terminal
