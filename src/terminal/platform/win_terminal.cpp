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
    if (hOut_ == INVALID_HANDLE_VALUE || hIn_ == INVALID_HANDLE_VALUE) {
        return std::unexpected("Failed to get standard handles");
    }
    return {};
}

auto WinTerminal::shutdown() -> void {
    // any extra shutdown logic
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
    // WriteConsoleA is largely unbuffered in the standard C sense, 
    // but we can leave this in case we wrap it in a stream later.
}

auto WinTerminal::poll_input(std::chrono::milliseconds timeout) -> std::optional<InputEvent> {
    // For now, simple wait and read one event
    DWORD result = WaitForSingleObject(hIn_, static_cast<DWORD>(timeout.count()));
    if (result == WAIT_OBJECT_0) {
        INPUT_RECORD record;
        DWORD read = 0;
        if (ReadConsoleInputW(hIn_, &record, 1, &read) && read > 0) {
            if (record.EventType == KEY_EVENT && record.Event.KeyEvent.bKeyDown) {
                // simple mapping for now
                KeyEvent ke{};
                ke.codepoint = record.Event.KeyEvent.uChar.UnicodeChar;
                if (record.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE) ke.key = Key::Escape;
                else ke.key = Key::Char; // TODO proper mapping
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
    if (!SetConsoleMode(hIn_, in_mode)) return std::unexpected("SetConsoleMode failed (in)");

    return {};
}

auto WinTerminal::disable_raw_mode() -> void {
    SetConsoleMode(hOut_, original_out_mode_);
    SetConsoleMode(hIn_, original_in_mode_);
}

auto WinTerminal::enable_mouse() -> void {
    DWORD mode;
    GetConsoleMode(hIn_, &mode);
    SetConsoleMode(hIn_, mode | ENABLE_MOUSE_INPUT);
    write(escape::enable_mouse_tracking());
}

auto WinTerminal::disable_mouse() -> void {
    DWORD mode;
    GetConsoleMode(hIn_, &mode);
    SetConsoleMode(hIn_, mode & ~ENABLE_MOUSE_INPUT);
    write(escape::disable_mouse_tracking());
}

auto WinTerminal::enter_alt_screen() -> void {
    write(escape::enter_alt_screen());
}

auto WinTerminal::leave_alt_screen() -> void {
    write(escape::leave_alt_screen());
}

} // namespace fugue::terminal::platform
#endif
