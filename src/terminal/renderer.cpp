#include "terminal/renderer.hpp"
#include "terminal/escape.hpp"
#include <string>

namespace fugue::terminal {

Renderer::Renderer(ITerminal& terminal) : terminal_(terminal) {}

auto Renderer::flush(ScreenBuffer& buffer) -> void {
    std::string frame;
    auto front = buffer.front();
    auto back = buffer.back();
    int cols = buffer.cols();
    int rows = buffer.rows();

    for (int r = 0; r < rows; ++r) {
        bool row_dirty = false;
        // Optimization: skip identical rows
        for (int c = 0; c < cols; ++c) {
            if (front[r * cols + c] == back[r * cols + c]) continue;
            
            row_dirty = true;
            const Cell& cell = back[r * cols + c];

            if (cursor_row_ != r || cursor_col_ != c) {
                frame += escape::move_to(c + 1, r + 1);
                cursor_row_ = r;
                cursor_col_ = c;
            }

            if (!(last_style_ == cell.style)) {
                frame += escape::apply_style(cell.style);
                last_style_ = cell.style;
            }

            char32_t cp = cell.codepoint;
            if (cp > 0x10FFFF || (cp >= 0xD800 && cp <= 0xDFFF)) cp = 0xFFFD;

            // Convert char32_t to UTF-8
            if (cp <= 0x7F) {
                frame += static_cast<char>(cp);
            } else if (cp <= 0x7FF) {
                frame += static_cast<char>(0xC0 | ((cp >> 6) & 0x1F));
                frame += static_cast<char>(0x80 | (cp & 0x3F));
            } else if (cp <= 0xFFFF) {
                frame += static_cast<char>(0xE0 | ((cp >> 12) & 0x0F));
                frame += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
                frame += static_cast<char>(0x80 | (cp & 0x3F));
            } else if (cp <= 0x10FFFF) {
                frame += static_cast<char>(0xF0 | ((cp >> 18) & 0x07));
                frame += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
                frame += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
                frame += static_cast<char>(0x80 | (cp & 0x3F));
            }

            cursor_col_ += cell.width;
            if (cell.width > 1) {
                c += cell.width - 1; // skip next cells if wide
            }
        }
    }

    if (!frame.empty()) {
        terminal_.write(frame);
    }
    
    buffer.swap(); // swap back and front buffers
}

auto Renderer::force_redraw(ScreenBuffer& buffer) -> void {
    last_style_ = Style{};
    cursor_col_ = -1;
    cursor_row_ = -1;
    terminal_.write(escape::reset_style() + escape::clear_screen());
    
    buffer.invalidate_front();
    flush(buffer);
}

} // namespace fugue::terminal
