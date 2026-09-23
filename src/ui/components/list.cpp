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

    bool needs_scrollbar = items_.size() > static_cast<size_t>(area.height);
    int content_width = needs_scrollbar ? area.width - 1 : area.width;

    for (int i = 0; i < area.height; ++i) {
        size_t item_idx = scroll_offset_ + i;
        if (item_idx >= items_.size()) {
            // clear remaining lines
            for (int c = area.x; c < area.x + content_width; ++c) {
                buffer.set_cell(c, area.y + i, terminal::Cell{U' ', normal_style_, 1});
            }
            continue;
        }

        const auto& style = (item_idx == selected_) ? selected_style_ : normal_style_;
        const auto& text = items_[item_idx];
        int len = std::min(content_width, static_cast<int>(text.size()));
        buffer.set_string(area.x, area.y + i, std::string_view(text).substr(0, len), style);

        // padding to fill width
        for (int c = area.x + len; c < area.x + content_width; ++c) {
            buffer.set_cell(c, area.y + i, terminal::Cell{U' ', style, 1});
        }
    }

    if (needs_scrollbar) {
        int thumb_size = std::max(1, static_cast<int>((area.height * area.height) / items_.size()));
        int track_size = area.height;
        int max_scroll = static_cast<int>(items_.size()) - area.height;
        int max_thumb_pos = track_size - thumb_size;
        int thumb_pos = (max_scroll > 0) ? static_cast<int>((scroll_offset_ * max_thumb_pos) / max_scroll) : 0;

        int sb_x = area.x + area.width - 1;
        for (int i = 0; i < area.height; ++i) {
            char32_t c = (i >= thumb_pos && i < thumb_pos + thumb_size) ? 0x2588 /* Full block */ : 0x2502 /* Vertical line */;
            buffer.set_cell(sb_x, area.y + i, terminal::Cell{c, normal_style_, 1});
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
