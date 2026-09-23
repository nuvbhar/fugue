#pragma once

#include "terminal/input_event.hpp"
#include <string>
#include <string_view>
#include <expected>
#include <chrono>
#include <optional>

namespace fugue::terminal {

struct Size {
    int cols{0};
    int rows{0};
};

class ITerminal {
public:
    virtual ~ITerminal() = default;
    
    virtual auto init() -> std::expected<void, std::string> = 0;
    virtual auto shutdown() -> void = 0;
    
    virtual auto size() const -> Size = 0;
    
    virtual auto write(std::string_view data) -> void = 0;
    virtual auto flush() -> void = 0;
    
    virtual auto poll_input(std::chrono::milliseconds timeout) -> std::optional<InputEvent> = 0;
    
    virtual auto enable_raw_mode() -> std::expected<void, std::string> = 0;
    virtual auto disable_raw_mode() -> void = 0;
    
    virtual auto enable_mouse() -> void = 0;
    virtual auto disable_mouse() -> void = 0;
    
    virtual auto enter_alt_screen() -> void = 0;
    virtual auto leave_alt_screen() -> void = 0;
};

} // namespace fugue::terminal
