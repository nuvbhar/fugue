#pragma once

#include <cstdint>

namespace fugue::terminal {

struct Color {
    enum class Type { Default, Indexed, RGB };
    Type type{Type::Default};
    uint8_t index{0};        // For Type::Indexed (0-255)
    uint8_t r{0}, g{0}, b{0}; // For Type::RGB

    static constexpr auto rgb(uint8_t r, uint8_t g, uint8_t b) -> Color {
        return Color{Type::RGB, 0, r, g, b};
    }

    static constexpr auto indexed(uint8_t index) -> Color {
        return Color{Type::Indexed, index, 0, 0, 0};
    }

    static constexpr auto default_color() -> Color {
        return Color{Type::Default, 0, 0, 0, 0};
    }

    auto operator==(const Color&) const -> bool = default;
};

struct Style {
    Color fg{Color::default_color()};
    Color bg{Color::default_color()};
    bool bold{false};
    bool italic{false};
    bool underline{false};
    bool dim{false};
    bool strikethrough{false};
    bool reverse{false};

    auto operator==(const Style&) const -> bool = default;
};

} // namespace fugue::terminal
