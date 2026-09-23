#include "ui/components/text_box.hpp"
#include <algorithm>

namespace fugue::ui {

TextBox::TextBox(std::string id, std::string text, terminal::Style style) 
    : id_(std::move(id)), text_(std::move(text)), style_(style) {}

auto TextBox::update(const core::EventBus& /*bus*/) -> void {
}

auto TextBox::render(terminal::ScreenBuffer& buffer, const Rect& area) -> void {
    if (area.width <= 0 || area.height <= 0) return;
    auto len = std::min<size_t>(text_.size(), static_cast<size_t>(area.width));
    buffer.set_string(area.x, area.y, std::string_view(text_).substr(0, len), style_);
}

auto TextBox::handle_input(const terminal::InputEvent& /*event*/) -> bool {
    return false;
}

auto TextBox::min_size() const -> terminal::Size {
    return terminal::Size{static_cast<int>(text_.size()), 1};
}

auto TextBox::id() const -> std::string_view {
    return id_;
}

auto TextBox::set_text(std::string text) -> void {
    text_ = std::move(text);
}

} // namespace fugue::ui
