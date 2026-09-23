#pragma once

#include "ui/layout_node.hpp"
#include "ui/rect.hpp"
#include <map>
#include <string>

namespace fugue::ui {

class LayoutEngine {
public:
    // Computes layout for the given node and container rect, returns a map of component_id to Rect
    static auto compute(const LayoutNode& root, const Rect& container) -> std::map<std::string, Rect>;
};

} // namespace fugue::ui
