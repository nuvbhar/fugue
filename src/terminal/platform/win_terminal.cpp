#include "terminal/platform/win_terminal.hpp"

#ifdef _WIN32
#include "terminal/escape.hpp"
#include <iostream>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

namespace fugue::terminal::platform {

WinTerminal::WinTerminal() {
    hOut_ = GetStdHandle(STD_OUTPUT_HANDLE);
    hIn_ = GetStdHandle(STD_INPUT_HANDLE);
}

WinTerminal::~WinTerminal() {
    shutdown();
}

auto WinTerminal::init() -> std::expected<void, std::string> {
    if (hOut_ == INVALID_HANDLE_VALUE || hOut_ == NULL || hIn_ == INVALID_HANDLE_VALUE || hIn_ == NULL) {
        return std::unexpected("Failed to get standard handles");
    }
    
    original_cp_ = GetConsoleOutputCP();
    SetConsoleOutputCP(CP_UTF8);
    
    return {};
}

auto WinTerminal::shutdown() -> void {
    if (is_mouse_enabled_) disable_mouse();
    if (is_alt_screen_) leave_alt_screen();
    if (is_raw_mode_) disable_raw_mode();
    if (original_cp_ != 0) SetConsoleOutputCP(original_cp_);
}

auto WinTerminal::size() const -> Size {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(hOut_, &csbi)) {
        return Size{
            csbi.srWindow.Right - csbi.srWindow.Left + 1,
            csbi.srWindow.Bottom - csbi.srWindow.Top + 1
        };
    }
    return Size{80, 24};
}

auto WinTerminal::write(std::string_view data) -> void {
    DWORD written = 0;
    WriteConsoleA(hOut_, data.data(), static_cast<DWORD>(data.size()), &written, nullptr);
}

auto WinTerminal::flush() -> void {
}

auto WinTerminal::poll_input(std::chrono::milliseconds timeout) -> std::optional<InputEvent> {
    DWORD result = WaitForSingleObject(hIn_, static_cast<DWORD>(timeout.count()));
    if (result == WAIT_OBJECT_0) {
        INPUT_RECORD record;
        DWORD read = 0;
        if (ReadConsoleInputW(hIn_, &record, 1, &read) && read > 0) {
            if (record.EventType == KEY_EVENT && record.Event.KeyEvent.bKeyDown) {
                KeyEvent ke{};
                ke.codepoint = record.Event.KeyEvent.uChar.UnicodeChar;
                
                switch (record.Event.KeyEvent.wVirtualKeyCode) {
                    case VK_ESCAPE: ke.key = Key::Escape; break;
                    case VK_RETURN: ke.key = Key::Enter; break;
                    case VK_BACK:   ke.key = Key::Backspace; break;
                    case VK_TAB:    ke.key = Key::Tab; break;
                    case VK_UP:     ke.key = Key::ArrowUp; break;
                    case VK_DOWN:   ke.key = Key::ArrowDown; break;
                    case VK_LEFT:   ke.key = Key::ArrowLeft; break;
                    case VK_RIGHT:  ke.key = Key::ArrowRight; break;
                    case VK_HOME:   ke.key = Key::Home; break;
                    case VK_END:    ke.key = Key::End; break;
                    case VK_PRIOR:  ke.key = Key::PageUp; break;
                    case VK_NEXT:   ke.key = Key::PageDown; break;
                    default:
                        ke.key = ke.codepoint != 0 ? Key::Char : Key::Unknown;
                }
                const DWORD st = record.Event.KeyEvent.dwControlKeyState;
                ke.mods.ctrl  = st & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED);
                ke.mods.alt   = st & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED);
                ke.mods.shift = st & SHIFT_PRESSED;
                
                return ke;
            } else if (record.EventType == WINDOW_BUFFER_SIZE_EVENT) {
                ResizeEvent re{};
                re.cols = record.Event.WindowBufferSizeEvent.dwSize.X;
                re.rows = record.Event.WindowBufferSizeEvent.dwSize.Y;
                return re;
            } else if (record.EventType == MOUSE_EVENT) {
                const auto& me = record.Event.MouseEvent;
                MouseEvent mouse_event{};
                mouse_event.col = me.dwMousePosition.X;
                mouse_event.row = me.dwMousePosition.Y;
                
                if (me.dwEventFlags == 0) { // button press or release
                    if (me.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
                        mouse_event.button = MouseButton::Left;
                        mouse_event.action = MouseAction::Press;
                    } else if (me.dwButtonState & RIGHTMOST_BUTTON_PRESSED) {
                        mouse_event.button = MouseButton::Right;
                        mouse_event.action = MouseAction::Press;
                    } else {
                        mouse_event.action = MouseAction::Release;
                    }
                } else if (me.dwEventFlags == MOUSE_MOVED) {
                    mouse_event.action = MouseAction::Move;
                }
                
                return mouse_event;
            }
        }
    }
    return std::nullopt;
}

auto WinTerminal::enable_raw_mode() -> std::expected<void, std::string> {
    if (!GetConsoleMode(hOut_, &original_out_mode_)) return std::unexpected("GetConsoleMode failed (out)");
    if (!GetConsoleMode(hIn_, &original_in_mode_)) return std::unexpected("GetConsoleMode failed (in)");

    DWORD out_mode = original_out_mode_ | ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN;
    if (!SetConsoleMode(hOut_, out_mode)) return std::unexpected("SetConsoleMode failed (out)");

    DWORD in_mode = original_in_mode_ & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT);
    in_mode |= ENABLE_WINDOW_INPUT;
    if (!SetConsoleMode(hIn_, in_mode)) {
        SetConsoleMode(hOut_, original_out_mode_);
        return std::unexpected("SetConsoleMode failed (in)");
    }
    
    is_raw_mode_ = true;
    return {};
}

auto WinTerminal::disable_raw_mode() -> void {
    if (!is_raw_mode_) return;
    SetConsoleMode(hOut_, original_out_mode_);
    SetConsoleMode(hIn_, original_in_mode_);
    is_raw_mode_ = false;
}

auto WinTerminal::enable_mouse() -> void {
    DWORD mode;
    GetConsoleMode(hIn_, &mode);
    SetConsoleMode(hIn_, mode | ENABLE_MOUSE_INPUT);
    write(escape::enable_mouse_tracking());
    is_mouse_enabled_ = true;
}

auto WinTerminal::disable_mouse() -> void {
    if (!is_mouse_enabled_) return;
    DWORD mode;
    GetConsoleMode(hIn_, &mode);
    SetConsoleMode(hIn_, mode & ~ENABLE_MOUSE_INPUT);
    write(escape::disable_mouse_tracking());
    is_mouse_enabled_ = false;
}

auto WinTerminal::enter_alt_screen() -> void {
    write(escape::enter_alt_screen());
    is_alt_screen_ = true;
}

auto WinTerminal::leave_alt_screen() -> void {
    if (!is_alt_screen_) return;
    write(escape::leave_alt_screen());
    is_alt_screen_ = false;
}

} // namespace fugue::terminal::platform
#endif
