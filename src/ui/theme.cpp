#include "ui/theme.hpp"

namespace fugue::ui {

static auto parse_color(const std::string& hex) -> terminal::Color {
    if (hex.length() == 7 && hex[0] == '#') {
        int r = std::stoi(hex.substr(1, 2), nullptr, 16);
        int g = std::stoi(hex.substr(3, 2), nullptr, 16);
        int b = std::stoi(hex.substr(5, 2), nullptr, 16);
        return terminal::Color::rgb(static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b));
    }
    return terminal::Color::default_color();
}

auto load_theme(const nlohmann::json& config) -> Theme {
    Theme theme = default_theme();
    
    if (config.contains("palette")) {
        for (const auto& [name, val] : config["palette"].items()) {
            if (val.is_string()) {
                theme.palette[name] = parse_color(val.get<std::string>());
            }
        }
    }
    
    // We can also parse text_styles and borders, but we'll stub it for now
    return theme;
}

auto default_theme() -> Theme {
    Theme theme;
    theme.palette["primary"] = terminal::Color::rgb(0, 255, 0);
    theme.palette["secondary"] = terminal::Color::rgb(100, 100, 100);
    theme.palette["background"] = terminal::Color::rgb(0, 0, 0);
    theme.palette["text"] = terminal::Color::rgb(255, 255, 255);

    theme.text_styles["title"] = terminal::Style{theme.palette["primary"], theme.palette["background"], true, false, false, false};
    theme.text_styles["artist"] = terminal::Style{theme.palette["secondary"], theme.palette["background"], false, true, false, false};
    theme.text_styles["status"] = terminal::Style{theme.palette["text"], theme.palette["background"], false, false, false, false};

    theme.default_border = BorderType::Rounded;
    theme.border_style = terminal::Style{theme.palette["primary"], theme.palette["background"], false, false, false, false};
    
    return theme;
}

} // namespace fugue::ui
