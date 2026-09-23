#pragma once

#include "terminal/style.hpp"
#include <string>

namespace fugue::terminal::escape {

// Cursor
auto move_to(int col, int row) -> std::string;    // ESC[{row};{col}H (1-indexed)
auto move_up(int n) -> std::string;                 // ESC[{n}A
auto move_down(int n) -> std::string;               // ESC[{n}B
auto hide_cursor() -> std::string;                  // ESC[?25l
auto show_cursor() -> std::string;                  // ESC[?25h

// Screen
auto enter_alt_screen() -> std::string;             // ESC[?1049h
auto leave_alt_screen() -> std::string;             // ESC[?1049l
auto clear_screen() -> std::string;                 // ESC[2J
auto clear_line() -> std::string;                   // ESC[2K

// Style
auto apply_style(const Style& style) -> std::string;  // SGR sequences
auto reset_style() -> std::string;                      // ESC[0m

// Color (within SGR)
auto fg_rgb(uint8_t r, uint8_t g, uint8_t b) -> std::string;  // ESC[38;2;r;g;bm
auto bg_rgb(uint8_t r, uint8_t g, uint8_t b) -> std::string;  // ESC[48;2;r;g;bm
auto fg_indexed(uint8_t index) -> std::string;                  // ESC[38;5;{n}m
auto bg_indexed(uint8_t index) -> std::string;                  // ESC[48;5;{n}m

// Mouse
auto enable_mouse_tracking() -> std::string;         // ESC[?1003h ESC[?1006h
auto disable_mouse_tracking() -> std::string;        // ESC[?1003l ESC[?1006l

} // namespace fugue::terminal::escape
