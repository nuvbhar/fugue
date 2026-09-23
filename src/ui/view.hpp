#pragma once

#include "ui/layout_node.hpp"
#include "ui/component.hpp"
#include <vector>
#include <string_view>

namespace fugue::ui {

class IView {
public:
    virtual ~IView() = default;
    
    virtual auto layout() const -> const LayoutNode& = 0;
    virtual auto components() -> std::vector<IComponent*> = 0;
    virtual auto name() const -> std::string_view = 0;
};

} // namespace fugue::ui
