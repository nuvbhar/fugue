#pragma once

#include "terminal/terminal.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

namespace fugue::terminal::platform {

class WinTerminal : public ITerminal {
public:
    WinTerminal();
    ~WinTerminal() override;

    auto init() -> std::expected<void, std::string> override;
    auto shutdown() -> void override;
    
    auto size() const -> Size override;
    
    auto write(std::string_view data) -> void override;
    auto flush() -> void override;
    
    auto poll_input(std::chrono::milliseconds timeout) -> std::optional<InputEvent> override;
    
    auto enable_raw_mode() -> std::expected<void, std::string> override;
    auto disable_raw_mode() -> void override;
    
    auto enable_mouse() -> void override;
    auto disable_mouse() -> void override;
    
    auto enter_alt_screen() -> void override;
    auto leave_alt_screen() -> void override;

private:
#ifdef _WIN32
    HANDLE hOut_{INVALID_HANDLE_VALUE};
    HANDLE hIn_{INVALID_HANDLE_VALUE};
    DWORD original_out_mode_{0};
    DWORD original_in_mode_{0};
    UINT original_cp_{0};
    bool is_raw_mode_{false};
    bool is_mouse_enabled_{false};
    bool is_alt_screen_{false};
#endif
};

} // namespace fugue::terminal::platform
