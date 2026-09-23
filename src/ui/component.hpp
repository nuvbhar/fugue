#pragma once

#include "ui/rect.hpp"
#include "terminal/screen_buffer.hpp"
#include "terminal/input_event.hpp"
#include "terminal/terminal.hpp"
#include <string_view>

// We need an EventBus later, but for now we'll stub it or forward declare
namespace fugue::core {
class EventBus;
}

namespace fugue::ui {

class IComponent {
public:
    virtual ~IComponent() = default;
    
    virtual auto update(const core::EventBus& bus) -> void = 0;
    virtual auto render(terminal::ScreenBuffer& buffer, const Rect& area) -> void = 0;
    virtual auto handle_input(const terminal::InputEvent& event) -> bool = 0;  // true = consumed
    virtual auto min_size() const -> terminal::Size = 0;
    virtual auto id() const -> std::string_view = 0;
};

} // namespace fugue::ui
