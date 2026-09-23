#pragma once

#include "ui/component.hpp"
#include <vector>
#include <string>

namespace fugue::ui {

class List : public IComponent {
public:
    explicit List(std::string id, terminal::Style normal_style, terminal::Style selected_style);
    
    auto update(const core::EventBus& bus) -> void override;
    auto render(terminal::ScreenBuffer& buffer, const Rect& area) -> void override;
    auto handle_input(const terminal::InputEvent& event) -> bool override;
    auto min_size() const -> terminal::Size override;
    auto id() const -> std::string_view override;

    auto set_items(std::vector<std::string> items) -> void;
    auto selected_index() const -> size_t;
    auto set_selected_index(size_t index) -> void;

private:
    std::string id_;
    std::vector<std::string> items_;
    size_t selected_{0};
    size_t scroll_offset_{0};
    terminal::Style normal_style_;
    terminal::Style selected_style_;
};

} // namespace fugue::ui
