#pragma once

#include "ui/component.hpp"
#include "ui/theme.hpp"
#include <memory>
#include <string>

namespace fugue::ui {

class Border : public IComponent {
public:
    explicit Border(std::string id, BorderType type, terminal::Style style, std::unique_ptr<IComponent> child);
    
    auto update(const core::EventBus& bus) -> void override;
    auto render(terminal::ScreenBuffer& buffer, const Rect& area) -> void override;
    auto handle_input(const terminal::InputEvent& event) -> bool override;
    auto min_size() const -> terminal::Size override;
    auto id() const -> std::string_view override;

private:
    std::string id_;
    BorderType type_;
    terminal::Style style_;
    std::unique_ptr<IComponent> child_;
};

} // namespace fugue::ui
