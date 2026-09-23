#include "ui/layout_engine.hpp"
#include <algorithm>

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

    int end_pos = current_pos + available_space;
    int prop_used = 0;
    size_t last_prop = node.children.size();
    for (size_t i = 0; i < node.children.size(); ++i) {
        if (node.children[i].policy == LayoutNode::SizePolicy::Proportional) last_prop = i;
    }

    for (size_t i = 0; i < node.children.size(); ++i) {
        const auto& child = node.children[i];
        int size = 0;
        if (child.policy == LayoutNode::SizePolicy::Proportional) {
            if (i == last_prop) {
                size = remaining_space - prop_used;
            } else if (total_proportional > 0.0f) {
                size = static_cast<int>((child.value / total_proportional) * remaining_space);
            }
            prop_used += size;
        } else {
            size = static_cast<int>(child.value);
        }
        size = std::clamp(size, 0, std::max(0, end_pos - current_pos));

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
