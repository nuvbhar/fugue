#pragma once

namespace fugue::ui {

struct Rect {
    int x{0};
    int y{0};
    int width{0};
    int height{0};

    [[nodiscard]] constexpr auto contains(int px, int py) const -> bool {
        return px >= x && px < x + width && py >= y && py < y + height;
    }

    [[nodiscard]] constexpr auto intersect(const Rect& other) const -> Rect {
        int nx = (x > other.x) ? x : other.x;
        int ny = (y > other.y) ? y : other.y;
        int right = (x + width < other.x + other.width) ? x + width : other.x + other.width;
        int bottom = (y + height < other.y + other.height) ? y + height : other.y + other.height;

        if (nx < right && ny < bottom) {
            return Rect{nx, ny, right - nx, bottom - ny};
        }
        return Rect{0, 0, 0, 0};
    }
    
    auto operator==(const Rect&) const -> bool = default;
};

} // namespace fugue::ui
