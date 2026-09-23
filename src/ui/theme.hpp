#pragma once

#include "terminal/style.hpp"
#include <map>
#include <string>
#include <nlohmann/json.hpp>

namespace fugue::ui {

enum class BorderType { None, Single, Double, Rounded, Ascii };

struct Theme {
    std::map<std::string, terminal::Color> palette;
    std::map<std::string, terminal::Style> text_styles;
    BorderType default_border{BorderType::Single};
    terminal::Style border_style{};
};

auto load_theme(const nlohmann::json& config) -> Theme;
auto default_theme() -> Theme;

} // namespace fugue::ui
