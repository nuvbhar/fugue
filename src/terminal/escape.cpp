#include "terminal/escape.hpp"
#include <format>

namespace fugue::terminal::escape {

auto move_to(int col, int row) -> std::string {
    return std::format("\x1b[{};{}H", row, col);
}

auto move_up(int n) -> std::string {
    return std::format("\x1b[{}A", n);
}

auto move_down(int n) -> std::string {
    return std::format("\x1b[{}B", n);
}

auto hide_cursor() -> std::string {
    return "\x1b[?25l";
}

auto show_cursor() -> std::string {
    return "\x1b[?25h";
}

auto enter_alt_screen() -> std::string {
    return "\x1b[?1049h";
}

auto leave_alt_screen() -> std::string {
    return "\x1b[?1049l";
}

auto clear_screen() -> std::string {
    return "\x1b[2J";
}

auto clear_line() -> std::string {
    return "\x1b[2K";
}

auto apply_style(const Style& style) -> std::string {
    std::string seq = "\x1b[0";
    if (style.bold) seq += ";1";
    if (style.dim) seq += ";2";
    if (style.italic) seq += ";3";
    if (style.underline) seq += ";4";
    if (style.reverse) seq += ";7";
    if (style.strikethrough) seq += ";9";

    if (style.fg.type == Color::Type::RGB) {
        seq += std::format(";38;2;{};{};{}", style.fg.r, style.fg.g, style.fg.b);
    } else if (style.fg.type == Color::Type::Indexed) {
        seq += std::format(";38;5;{}", style.fg.index);
    }

    if (style.bg.type == Color::Type::RGB) {
        seq += std::format(";48;2;{};{};{}", style.bg.r, style.bg.g, style.bg.b);
    } else if (style.bg.type == Color::Type::Indexed) {
        seq += std::format(";48;5;{}", style.bg.index);
    }

    seq += "m";
    return seq;
}

auto reset_style() -> std::string {
    return "\x1b[0m";
}

auto fg_rgb(uint8_t r, uint8_t g, uint8_t b) -> std::string {
    return std::format("\x1b[38;2;{};{};{}m", r, g, b);
}

auto bg_rgb(uint8_t r, uint8_t g, uint8_t b) -> std::string {
    return std::format("\x1b[48;2;{};{};{}m", r, g, b);
}

auto fg_indexed(uint8_t index) -> std::string {
    return std::format("\x1b[38;5;{}m", index);
}

auto bg_indexed(uint8_t index) -> std::string {
    return std::format("\x1b[48;5;{}m", index);
}

auto enable_mouse_tracking() -> std::string {
    return "\x1b[?1003h\x1b[?1006h";
}

auto disable_mouse_tracking() -> std::string {
    return "\x1b[?1003l\x1b[?1006l";
}

} // namespace fugue::terminal::escape
