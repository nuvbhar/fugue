#pragma once

#include "terminal/screen_buffer.hpp"
#include "terminal/terminal.hpp"

namespace fugue::terminal {

class Renderer {
public:
    explicit Renderer(ITerminal& terminal);
    
    /// Diff the screen buffer and emit changes to the terminal.
    auto flush(ScreenBuffer& buffer) -> void;
    
    /// Force full redraw (e.g., after terminal corruption).
    auto force_redraw(ScreenBuffer& buffer) -> void;
    
private:
    ITerminal& terminal_;
    Style last_style_{};
    int cursor_col_{-1}, cursor_row_{-1};
};

} // namespace fugue::terminal
