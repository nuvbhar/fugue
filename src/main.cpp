#include <iostream>
#include <thread>
#include <chrono>
#include <format>
#include <map>

#include "terminal/platform/win_terminal.hpp"
#include "terminal/screen_buffer.hpp"
#include "terminal/renderer.hpp"

#include "ui/theme.hpp"
#include "ui/layout_node.hpp"
#include "ui/layout_engine.hpp"
#include "ui/components/text_box.hpp"
#include "ui/components/border.hpp"
#include "ui/components/list.hpp"

using namespace std::chrono_literals;
using namespace fugue::terminal;
using namespace fugue::ui;

int main() {
    fugue::terminal::platform::WinTerminal term;
    if (!term.init().has_value()) {
        std::cerr << "Failed to init terminal\n";
        return 1;
    }

    if (!term.enable_raw_mode().has_value()) {
        std::cerr << "Failed to enable raw mode\n";
        return 1;
    }

    term.enter_alt_screen();
    term.enable_mouse();
    term.hide_cursor();

    auto size = term.size();
    ScreenBuffer buffer(size.cols, size.rows);
    Renderer renderer(term);

    auto theme = default_theme();

    // Create components
    auto title_text = std::make_unique<TextBox>("title_text", "Fugue v0.1", theme.text_styles["title"]);
    auto title_border = std::make_unique<Border>("title", BorderType::Rounded, theme.border_style, std::move(title_text));

    auto list_comp = std::make_unique<List>("track_list", theme.text_styles["status"], theme.text_styles["artist"]);
    std::vector<std::string> tracks;
    for (int i = 1; i <= 50; ++i) {
        tracks.push_back(std::format("Track {}: Sample Track Name", i));
    }
    list_comp->set_items(tracks);
    auto main_border = std::make_unique<Border>("main_panel", BorderType::Single, theme.border_style, std::move(list_comp));

    std::map<std::string, IComponent*> components;
    components["title"] = title_border.get();
    components["main_panel"] = main_border.get();

    // Create layout tree
    LayoutNode root;
    root.direction = LayoutNode::Direction::Column;
    
    LayoutNode title_node;
    title_node.policy = LayoutNode::SizePolicy::Fixed;
    title_node.value = 3.0f;
    title_node.component_id = "title";

    LayoutNode main_node;
    main_node.policy = LayoutNode::SizePolicy::Proportional;
    main_node.value = 1.0f;
    main_node.component_id = "main_panel";

    root.children.push_back(title_node);
    root.children.push_back(main_node);

    bool running = true;
    while (running) {
        // Poll input
        while (auto event = term.poll_input(10ms)) {
            if (auto* key = std::get_if<KeyEvent>(&*event)) {
                if (key->key == Key::Escape || key->codepoint == U'q') running = false;
                else components["main_panel"]->handle_input(*event); // forward input to main panel (List)
            } else if (auto* resize = std::get_if<ResizeEvent>(&*event)) {
                buffer.resize(resize->cols, resize->rows);
                renderer.force_redraw(buffer);
            }
        }

        buffer.clear();
        
        // Layout
        fugue::ui::Rect container{0, 0, buffer.cols(), buffer.rows()};
        auto layout_map = LayoutEngine::compute(root, container);

        // Render
        for (const auto& [id, rect] : layout_map) {
            if (components.count(id)) {
                components[id]->render(buffer, rect);
            }
        }

        renderer.flush(buffer);
        std::this_thread::sleep_for(16ms);
    }

    return 0;
}
