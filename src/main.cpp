#include <iostream>
#include <thread>
#include <chrono>
#include <format>

#include "terminal/platform/win_terminal.hpp"
#include "terminal/screen_buffer.hpp"
#include "terminal/renderer.hpp"

using namespace std::chrono_literals;
using namespace fugue::terminal;

int main() {
    platform::WinTerminal term;
    if (!term.init().has_value()) {
        std::cerr << "Failed to init terminal\n";
        return 1;
    }

    term.enable_raw_mode();
    term.enter_alt_screen();
    term.enable_mouse();

    auto size = term.size();
    ScreenBuffer buffer(size.cols, size.rows);
    Renderer renderer(term);

    int box_x = size.cols / 2 - 5;
    int box_y = size.rows / 2 - 2;

    bool running = true;
    while (running) {
        // Poll input
        while (auto event = term.poll_input(10ms)) {
            if (auto* key = std::get_if<KeyEvent>(&*event)) {
                if (key->key == Key::Escape) running = false;
                else if (key->key == Key::ArrowUp) box_y--;
                else if (key->key == Key::ArrowDown) box_y++;
                else if (key->key == Key::ArrowLeft) box_x--;
                else if (key->key == Key::ArrowRight) box_x++;
                else if (key->codepoint == U'q') running = false;
            } else if (auto* mouse = std::get_if<MouseEvent>(&*event)) {
                if (mouse->action == MouseAction::Press) {
                    std::string msg = std::format("Mouse: {}, {}", mouse->col, mouse->row);
                    buffer.set_string(1, 1, msg, Style{});
                }
            } else if (auto* resize = std::get_if<ResizeEvent>(&*event)) {
                buffer.resize(resize->cols, resize->rows);
            }
        }

        // Draw
        buffer.clear();
        
        Style box_style;
        box_style.fg = Color::rgb(0, 255, 0); // Green
        box_style.bg = Color::rgb(0, 0, 255); // Blue background
        
        fugue::ui::Rect box_rect{box_x, box_y, 10, 5};
        buffer.fill(box_rect, Cell{U'#', box_style, 1});

        renderer.flush(buffer);
        std::this_thread::sleep_for(16ms);
    }

    term.disable_mouse();
    term.leave_alt_screen();
    term.disable_raw_mode();
    term.shutdown();

    return 0;
}
