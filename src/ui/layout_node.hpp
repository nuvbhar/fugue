#pragma once

#include <vector>
#include <string>

namespace fugue::ui {

struct LayoutNode {
    enum class Direction { Row, Column };
    enum class SizePolicy { Fixed, Proportional, Min };
    Direction direction{Direction::Row};
    SizePolicy policy{SizePolicy::Proportional};
    float value{1.0f};  // pixels for Fixed, weight for Proportional
    std::vector<LayoutNode> children;
    std::string component_id;  // leaf nodes reference a component
};

} // namespace fugue::ui
