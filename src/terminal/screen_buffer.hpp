#pragma once

#include "terminal/cell.hpp"
#include "ui/rect.hpp"

#include <vector>
#include <span>
#include <string_view>

namespace fugue::terminal {

class ScreenBuffer {
public:
    explicit ScreenBuffer(int cols, int rows);

    auto resize(int cols, int rows) -> void;
    auto cols() const -> int;
    auto rows() const -> int;

    // Write to back buffer
    auto set_cell(int col, int row, const Cell& cell) -> void;
    auto set_string(int col, int row, std::string_view text, const Style& style) -> void;
    auto fill(const ui::Rect& area, const Cell& cell) -> void;
    auto clear() -> void;

    // Access (from front buffer or back buffer? Usually from back buffer)
    auto at(int col, int row) const -> const Cell&;

    // Buffer management
    auto swap() -> void;        // Back becomes front, allocate new back
    auto front() const -> std::span<const Cell>;
    auto back() const -> std::span<const Cell>;

private:
    int cols_{0}, rows_{0};
    std::vector<Cell> front_buffer_;
    std::vector<Cell> back_buffer_;
};

} // namespace fugue::terminal
