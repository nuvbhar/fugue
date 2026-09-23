#include "terminal/screen_buffer.hpp"

namespace fugue::terminal {

ScreenBuffer::ScreenBuffer(int cols, int rows) : cols_(cols), rows_(rows) {
    front_buffer_.resize(cols * rows, Cell{});
    back_buffer_.resize(cols * rows, Cell{});
}

auto ScreenBuffer::resize(int cols, int rows) -> void {
    if (cols == cols_ && rows == rows_) return;
    
    std::vector<Cell> new_front(cols * rows, Cell{});
    std::vector<Cell> new_back(cols * rows, Cell{});

    int min_cols = std::min(cols, cols_);
    int min_rows = std::min(rows, rows_);

    for (int r = 0; r < min_rows; ++r) {
        for (int c = 0; c < min_cols; ++c) {
            new_front[r * cols + c] = front_buffer_[r * cols_ + c];
            new_back[r * cols + c] = back_buffer_[r * cols_ + c];
        }
    }

    cols_ = cols;
    rows_ = rows;
    front_buffer_ = std::move(new_front);
    back_buffer_ = std::move(new_back);
}

auto ScreenBuffer::cols() const -> int {
    return cols_;
}

auto ScreenBuffer::rows() const -> int {
    return rows_;
}

auto ScreenBuffer::set_cell(int col, int row, const Cell& cell) -> void {
    if (col >= 0 && col < cols_ && row >= 0 && row < rows_) {
        back_buffer_[row * cols_ + col] = cell;
    }
}

auto ScreenBuffer::set_string(int col, int row, std::string_view text, const Style& style) -> void {
    int c = col;
    for (char32_t ch : text) { // Note: iterating string_view yields char, for proper char32_t we'd need utf8 decoding. We'll cast for now.
        if (c >= cols_) break;
        set_cell(c++, row, Cell{static_cast<char32_t>(ch), style, 1});
    }
}

auto ScreenBuffer::fill(const ui::Rect& area, const Cell& cell) -> void {
    for (int r = area.y; r < area.y + area.height; ++r) {
        for (int c = area.x; c < area.x + area.width; ++c) {
            set_cell(c, r, cell);
        }
    }
}

auto ScreenBuffer::clear() -> void {
    std::fill(back_buffer_.begin(), back_buffer_.end(), Cell{});
}

auto ScreenBuffer::at(int col, int row) const -> const Cell& {
    static Cell empty_cell{};
    if (col >= 0 && col < cols_ && row >= 0 && row < rows_) {
        return back_buffer_[row * cols_ + col];
    }
    return empty_cell;
}

auto ScreenBuffer::swap() -> void {
    front_buffer_ = back_buffer_;
}

auto ScreenBuffer::front() const -> std::span<const Cell> {
    return front_buffer_;
}

auto ScreenBuffer::back() const -> std::span<const Cell> {
    return back_buffer_;
}

} // namespace fugue::terminal
