#pragma once

#include "terminal/style.hpp"

#include <cstdint>

namespace fugue::terminal {

struct Cell {
    char32_t codepoint{U' '};
    Style style{};
    uint8_t width{1};        // 1 for normal, 2 for wide (CJK) chars

    auto operator==(const Cell&) const -> bool = default;
};

} // namespace fugue::terminal
