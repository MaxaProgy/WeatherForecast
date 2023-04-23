
// #include <stdlib.h>  // for EXIT_SUCCESS

// #include <ftxui/dom/elements.hpp>  // for operator|=, Element, bgcolor,
// color, graph, border #include <ftxui/dom/table.hpp> #include
// <ftxui/screen/screen.hpp>  // for Fixed, Screen #include <iostream> // for
// endl, cout, ostream #include <vector>                   // for vector

// #include "ftxui/component/component.hpp"
// #include "ftxui/component/component.hpp"  // for Radiobox
// #include "ftxui/component/screen_interactive.hpp"
// #include "ftxui/dom/node.hpp"  // for Render
// #include "ftxui/screen/color.hpp"
// std::string IMAGES_WEATHER[] = {};

// namespace weather_forecast {
// using namespace ftxui;

// class Interface {};

// };  // namespace weather_forecast

// ftxui::Element MakeViewDay(const std::string& title) {
//     using namespace ftxui;

//     return window(text(title) | hcenter | bold,
//                   hbox({vbox({text("qqqqqqqq") | hcenter, separator(),
//                               vbox({text("ssssssssss"),
//                                     text("ssssssssss") | align_right})}) |
//                             flex_grow,
//                         separator(),
//                         vbox({text("qqqqqqqq") | hcenter, separator(),
//                               vbox({text("ssssssssss"),
//                                     text("ssssssssss") | align_right})}) |
//                             flex_grow,
//                         separator(),
//                         vbox({text("qqqqqqqq") | hcenter, separator(),
//                               vbox({text("ssssssssss"),
//                                     text("ssssssssss") | align_right})}) |
//                             flex_grow}));
// };

// int main() {
//     using namespace ftxui;

//     auto screen = ScreenInteractive::TerminalOutput();
//     auto renderer = Renderer([] { return MakeViewDay("My interface"); });

//     auto component = CatchEvent(renderer, [&](Event event) {
//         if (event == Event::Character('q')) {
//             screen.ExitLoopClosure()();
//             return true;
//         }

//         if (event == Event::ArrowDown) {
//             return true;
//         }
//         return false;
//     });
//     screen.Loop(component);

//     // Element document =
//     //     vbox({MakeViewDay("Thu 13 April"), MakeViewDay("Thu 14 April")});

//     // document |= color(Color::Blue);

//     // auto screen = Screen::Create(Dimension::Full(), Dimension::Full());
//     // Render(screen, document);
//     // screen.Print();
//     std::cout << std::endl;
//     return 0;
// }
// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#include <stddef.h>  // for size_t

#include <algorithm>  // for max
#include <memory>     // for allocator, shared_ptr
#include <string>  // for char_traits, operator+, string, basic_string, to_string
#include <utility>  // for move
#include <vector>   // for vector

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"       // for CatchEvent, Renderer
#include "ftxui/component/event.hpp"           // for Event
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Left, Mouse::Middle, Mouse::None, Mouse::Pressed, Mouse::Released, Mouse::Right, Mouse::WheelDown, Mouse::WheelUp
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for text, vbox, window, Element, Elements

using namespace ftxui;

ftxui::Element MakeViewDay(const std::string& title) {
    using namespace ftxui;

    return window(text(title) | hcenter | bold,
                  hbox({vbox({text("qqqqqqqq") | hcenter, separator(),
                              vbox({text("ssssssssss"),
                                    text("ssssssssss") | align_right})}) |
                            flex_grow,
                        separator(),
                        vbox({text("qqqqqqqq") | hcenter, separator(),
                              vbox({text("ssssssssss"),
                                    text("ssssssssss") | align_right})}) |
                            flex_grow,
                        separator(),
                        vbox({text("qqqqqqqq") | hcenter, separator(),
                              vbox({text("ssssssssss"),
                                    text("ssssssssss") | align_right})}) |
                            flex_grow}));
};

int main() {
    using namespace ftxui;

    auto screen = ScreenInteractive::Fullscreen();

    std::vector<std::string> v{"sdfs", "ddddddd", "ppppp"};

    int c = 0;
    auto renderer = Renderer([&] { return MakeViewDay(v[c]); });
    auto component = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Character('q')) {
            screen.ExitLoopClosure()();
            return true;
        }

        if (event == Event::ArrowDown) {
            c = (c + 1) % v.size();
            return true;
        }
        return false;
    });
    screen.Loop(component);
    return 0;
}