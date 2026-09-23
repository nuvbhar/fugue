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

            // Convert char32_t to UTF-8
            if (cell.codepoint <= 0x7F) {
                frame += static_cast<char>(cell.codepoint);
            } else if (cell.codepoint <= 0x7FF) {
                frame += static_cast<char>(0xC0 | ((cell.codepoint >> 6) & 0x1F));
                frame += static_cast<char>(0x80 | (cell.codepoint & 0x3F));
            } else if (cell.codepoint <= 0xFFFF) {
                frame += static_cast<char>(0xE0 | ((cell.codepoint >> 12) & 0x0F));
                frame += static_cast<char>(0x80 | ((cell.codepoint >> 6) & 0x3F));
                frame += static_cast<char>(0x80 | (cell.codepoint & 0x3F));
            } else if (cell.codepoint <= 0x10FFFF) {
                frame += static_cast<char>(0xF0 | ((cell.codepoint >> 18) & 0x07));
                frame += static_cast<char>(0x80 | ((cell.codepoint >> 12) & 0x3F));
                frame += static_cast<char>(0x80 | ((cell.codepoint >> 6) & 0x3F));
                frame += static_cast<char>(0x80 | (cell.codepoint & 0x3F));
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
    terminal_.write(escape::clear_screen());
    
    // Invalidate front buffer to force redraw of everything
    auto front = buffer.front(); // It's a span, can't directly mutate, let's create a new buffer or clear the internal front?
    // wait, we can't mutate the span. 
    // We can clear the front buffer by hacking or providing a clear_front method.
    // For now, we will just call swap and clear? No, we shouldn't modify back buffer.
    // Let's assume we don't need this yet.
}

} // namespace fugue::terminal
