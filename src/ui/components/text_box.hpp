#pragma once

#include "ui/component.hpp"
#include <string>

namespace fugue::ui {

class TextBox : public IComponent {
public:
    explicit TextBox(std::string id, std::string text, terminal::Style style);
    
    auto update(const core::EventBus& bus) -> void override;
    auto render(terminal::ScreenBuffer& buffer, const Rect& area) -> void override;
    auto handle_input(const terminal::InputEvent& event) -> bool override;
    auto min_size() const -> terminal::Size override;
    auto id() const -> std::string_view override;

    auto set_text(std::string text) -> void;

private:
    std::string id_;
    std::string text_;
    terminal::Style style_;
};

} // namespace fugue::ui
