#include "ui/theme.hpp"
#include <charconv>

namespace fugue::ui {

static auto parse_color(const std::string& hex) -> terminal::Color {
    if (hex.length() != 7 || hex[0] != '#') return terminal::Color::default_color();
    uint8_t c[3]{};
    for (int i = 0; i < 3; ++i) {
        const char* first = hex.data() + 1 + i * 2;
        auto [ptr, ec] = std::from_chars(first, first + 2, c[i], 16);
        if (ec != std::errc{} || ptr != first + 2) return terminal::Color::default_color();
    }
    return terminal::Color::rgb(c[0], c[1], c[2]);
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
    theme.palette["primary"] = terminal::Color::rgb(153, 168, 158);   // Soft pastel grayish-green
    theme.palette["secondary"] = terminal::Color::rgb(100, 100, 100);
    theme.palette["background"] = terminal::Color::default_color();
    theme.palette["text"] = terminal::Color::rgb(215, 215, 215);      // Soft off-white

    theme.text_styles["title"] = terminal::Style{theme.palette["primary"], theme.palette["background"], true, false, false, false};
    theme.text_styles["artist"] = terminal::Style{theme.palette["secondary"], theme.palette["background"], false, true, false, false};
    theme.text_styles["status"] = terminal::Style{theme.palette["text"], theme.palette["background"], false, false, false, false};

    theme.default_border = BorderType::Rounded;
    theme.border_style = terminal::Style{theme.palette["primary"], theme.palette["background"], false, false, false, false};
    
    return theme;
}

} // namespace fugue::ui
