#pragma once

#include "terminal/input_event.hpp"
#include <string_view>
#include <vector>

namespace fugue::terminal {

class InputParser {
public:
    auto parse(std::string_view data) -> std::vector<InputEvent>;
};

} // namespace fugue::terminal
