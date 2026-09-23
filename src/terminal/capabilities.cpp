#include "terminal/capabilities.hpp"
#include <cstdlib>

namespace fugue::terminal {

auto detect_capabilities() -> TerminalCapabilities {
    TerminalCapabilities caps;
    caps.unicode = true; // Modern assumption
    
    const char* colorterm = std::getenv("COLORTERM");
    if (colorterm && (std::string(colorterm) == "truecolor" || std::string(colorterm) == "24bit")) {
        caps.truecolor = true;
    }
    
#ifdef _WIN32
    // Windows Terminal supports truecolor
    if (std::getenv("WT_SESSION") != nullptr) {
        caps.truecolor = true;
    }
#endif

    const char* term_program = std::getenv("TERM_PROGRAM");
    if (term_program) {
        caps.term_program = term_program;
        if (caps.term_program == "kitty") {
            caps.image_protocol = ImageProtocol::Kitty;
        } else if (caps.term_program == "iTerm.app" || caps.term_program == "WezTerm") {
            caps.image_protocol = ImageProtocol::ITerm2;
        }
    }

    // Default to Braille if nothing else matched
    if (caps.image_protocol == ImageProtocol::None) {
        caps.image_protocol = ImageProtocol::Braille;
    }

    return caps;
}

} // namespace fugue::terminal
