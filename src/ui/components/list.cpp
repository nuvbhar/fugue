#include "ui/components/list.hpp"
#include <algorithm>

namespace fugue::ui {

List::List(std::string id, terminal::Style normal_style, terminal::Style selected_style)
    : id_(std::move(id)), normal_style_(normal_style), selected_style_(selected_style) {}

auto List::update(const core::EventBus& /*bus*/) -> void {
}

auto List::render(terminal::ScreenBuffer& buffer, const Rect& area) -> void {
    if (area.width <= 0 || area.height <= 0) return;

    if (selected_ < scroll_offset_) {
        scroll_offset_ = selected_;
    } else if (selected_ >= scroll_offset_ + area.height) {
        scroll_offset_ = selected_ - area.height + 1;
    }

    for (int i = 0; i < area.height; ++i) {
        size_t item_idx = scroll_offset_ + i;
        if (item_idx >= items_.size()) break;

        const auto& style = (item_idx == selected_) ? selected_style_ : normal_style_;
        buffer.set_string(area.x, area.y + i, items_[item_idx], style);
        
        // padding to fill width
        int len = static_cast<int>(items_[item_idx].size());
        for (int c = area.x + len; c < area.x + area.width; ++c) {
            buffer.set_cell(c, area.y + i, terminal::Cell{U' ', style, 1});
        }
    }
}

auto List::handle_input(const terminal::InputEvent& event) -> bool {
    if (auto* key = std::get_if<terminal::KeyEvent>(&event)) {
        if (key->key == terminal::Key::ArrowUp) {
            if (selected_ > 0) {
                selected_--;
                return true;
            }
        } else if (key->key == terminal::Key::ArrowDown) {
            if (selected_ + 1 < items_.size()) {
                selected_++;
                return true;
            }
        }
    }
    return false;
}

auto List::min_size() const -> terminal::Size {
    int max_width = 0;
    for (const auto& item : items_) {
        max_width = std::max(max_width, static_cast<int>(item.size()));
    }
    return terminal::Size{max_width, static_cast<int>(items_.size())};
}

auto List::id() const -> std::string_view {
    return id_;
}

auto List::set_items(std::vector<std::string> items) -> void {
    items_ = std::move(items);
    if (selected_ >= items_.size() && !items_.empty()) {
        selected_ = items_.size() - 1;
    } else if (items_.empty()) {
        selected_ = 0;
    }
}

auto List::selected_index() const -> size_t {
    return selected_;
}

auto List::set_selected_index(size_t index) -> void {
    if (index < items_.size()) {
        selected_ = index;
    }
}

} // namespace fugue::ui
