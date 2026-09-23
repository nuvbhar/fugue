#include "ui/components/border.hpp"
#include <algorithm>

namespace fugue::ui {

Border::Border(std::string id, BorderType type, terminal::Style style, std::unique_ptr<IComponent> child)
    : id_(std::move(id)), type_(type), style_(style), child_(std::move(child)) {}

auto Border::update(const core::EventBus& bus) -> void {
    if (child_) {
        child_->update(bus);
    }
}

auto Border::render(terminal::ScreenBuffer& buffer, const Rect& area) -> void {
    if (area.width <= 0 || area.height <= 0) return;

    if (type_ != BorderType::None) {
        char32_t tl = U'+', tr = U'+', bl = U'+', br = U'+';
        char32_t h = U'-', v = U'|';

        if (type_ == BorderType::Single || type_ == BorderType::Rounded) {
            h = 0x2500; v = 0x2502; // U'─', U'│'
            tl = (type_ == BorderType::Rounded) ? 0x256D : 0x250C; // U'╭' or U'┌'
            tr = (type_ == BorderType::Rounded) ? 0x256E : 0x2510; // U'╮' or U'┐'
            bl = (type_ == BorderType::Rounded) ? 0x2570 : 0x2514; // U'╰' or U'└'
            br = (type_ == BorderType::Rounded) ? 0x256F : 0x2518; // U'╯' or U'┘'
        } else if (type_ == BorderType::Double) {
            h = 0x2550; v = 0x2551; // U'═', U'║'
            tl = 0x2554; tr = 0x2557; // U'╔', U'╗'
            bl = 0x255A; br = 0x255D; // U'╚', U'╝'
        }

        // Draw corners
        buffer.set_cell(area.x, area.y, terminal::Cell{tl, style_, 1});
        buffer.set_cell(area.x + area.width - 1, area.y, terminal::Cell{tr, style_, 1});
        buffer.set_cell(area.x, area.y + area.height - 1, terminal::Cell{bl, style_, 1});
        buffer.set_cell(area.x + area.width - 1, area.y + area.height - 1, terminal::Cell{br, style_, 1});

        // Draw top/bottom
        for (int c = area.x + 1; c < area.x + area.width - 1; ++c) {
            buffer.set_cell(c, area.y, terminal::Cell{h, style_, 1});
            buffer.set_cell(c, area.y + area.height - 1, terminal::Cell{h, style_, 1});
        }

        // Draw left/right
        for (int r = area.y + 1; r < area.y + area.height - 1; ++r) {
            buffer.set_cell(area.x, r, terminal::Cell{v, style_, 1});
            buffer.set_cell(area.x + area.width - 1, r, terminal::Cell{v, style_, 1});
        }
    }

    if (child_) {
        Rect child_area = area;
        if (type_ != BorderType::None) {
            child_area.x += 1;
            child_area.y += 1;
            child_area.width = std::max(0, child_area.width - 2);
            child_area.height = std::max(0, child_area.height - 2);
        }
        child_->render(buffer, child_area);
    }
}

auto Border::handle_input(const terminal::InputEvent& event) -> bool {
    if (child_) {
        return child_->handle_input(event);
    }
    return false;
}

auto Border::min_size() const -> terminal::Size {
    terminal::Size ms{0, 0};
    if (child_) {
        ms = child_->min_size();
    }
    if (type_ != BorderType::None) {
        ms.cols += 2;
        ms.rows += 2;
    }
    return ms;
}

auto Border::id() const -> std::string_view {
    return id_;
}

} // namespace fugue::ui
