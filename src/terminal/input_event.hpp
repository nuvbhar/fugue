#pragma once

#include <variant>
#include <cstdint>

namespace fugue::terminal {

enum class Key {
    Unknown, Enter, Escape, Backspace, Tab, Space,
    ArrowUp, ArrowDown, ArrowLeft, ArrowRight,
    Home, End, PageUp, PageDown,
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
    Char
};

struct Modifiers {
    bool ctrl{false};
    bool alt{false};
    bool shift{false};
};

struct KeyEvent {
    Key key{Key::Unknown};
    Modifiers mods{};
    char32_t codepoint{U'\0'};
};

enum class MouseButton { Left, Middle, Right, WheelUp, WheelDown, None };
enum class MouseAction { Press, Release, Move };

struct MouseEvent {
    int col{0};
    int row{0};
    MouseButton button{MouseButton::None};
    MouseAction action{MouseAction::Move};
};

struct ResizeEvent {
    int cols{0};
    int rows{0};
};

using InputEvent = std::variant<KeyEvent, MouseEvent, ResizeEvent>;

} // namespace fugue::terminal
