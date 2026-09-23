#include "terminal/input.hpp"

namespace fugue::terminal {

auto InputParser::parse(std::string_view data) -> std::vector<InputEvent> {
    std::vector<InputEvent> events;
    // Basic stub, real VT parsing goes here.
    for (char c : data) {
        KeyEvent ke{};
        ke.codepoint = static_cast<char32_t>(c);
        ke.key = Key::Char;
        events.push_back(ke);
    }
    return events;
}

} // namespace fugue::terminal
