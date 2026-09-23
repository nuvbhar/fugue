#include "ui/layout_engine.hpp"

namespace fugue::ui {

static auto compute_impl(const LayoutNode& node, const Rect& container, std::map<std::string, Rect>& out) -> void {
    if (node.children.empty()) {
        if (!node.component_id.empty()) {
            out[node.component_id] = container;
        }
        return;
    }

    float total_proportional = 0.0f;
    int total_fixed = 0;

    for (const auto& child : node.children) {
        if (child.policy == LayoutNode::SizePolicy::Proportional) {
            total_proportional += child.value;
        } else if (child.policy == LayoutNode::SizePolicy::Fixed) {
            total_fixed += static_cast<int>(child.value);
        } else if (child.policy == LayoutNode::SizePolicy::Min) {
            // For now, Min policy will just take its fixed value minimum.
            total_fixed += static_cast<int>(child.value);
        }
    }

    int available_space = (node.direction == LayoutNode::Direction::Row) ? container.width : container.height;
    int remaining_space = std::max(0, available_space - total_fixed);
    
    int current_pos = (node.direction == LayoutNode::Direction::Row) ? container.x : container.y;

    for (const auto& child : node.children) {
        int size = 0;
        if (child.policy == LayoutNode::SizePolicy::Proportional) {
            if (total_proportional > 0.0f) {
                size = static_cast<int>((child.value / total_proportional) * remaining_space);
            }
        } else {
            size = static_cast<int>(child.value);
        }

        Rect child_rect = container;
        if (node.direction == LayoutNode::Direction::Row) {
            child_rect.x = current_pos;
            child_rect.width = size;
            current_pos += size;
        } else {
            child_rect.y = current_pos;
            child_rect.height = size;
            current_pos += size;
        }

        compute_impl(child, child_rect, out);
    }
}

auto LayoutEngine::compute(const LayoutNode& root, const Rect& container) -> std::map<std::string, Rect> {
    std::map<std::string, Rect> result;
    compute_impl(root, container, result);
    return result;
}

} // namespace fugue::ui
