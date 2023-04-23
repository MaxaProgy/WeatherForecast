#pragma once
#include "weather_forecast.h"

#include <cpr/cpr.h>

#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <vector>

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"       // for CatchEvent, Renderer
#include "ftxui/component/event.hpp"           // for Event
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Left, Mouse::Middle, Mouse::None, Mouse::Pressed, Mouse::Released, Mouse::Right, Mouse::WheelDown, Mouse::WheelUp
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"

namespace weather_forecast {

WeatherForecast::WeatherForecast(const std::string& path_to_config) {
    std::ifstream f(path_to_config, std::ifstream::binary);

    if (!f) {
        throw std::invalid_argument{"File \"" + path_to_config +
                                    "\" not found."};
    }
    nlohmann::json data = nlohmann::json::parse(f);
    f.close();

    for (const auto& city : data["cities"]) {
        cities.AddCity(city["name"], city["count_days"]);
        UpdateWeatherForCity(cities.Back());
    }

    Print();
}

void WeatherForecast::UpdateWeatherForCity(City& city) {
    // todo параметры выбирать надо
    cpr::Response r =
        cpr::Get(cpr::Url{"https://api.open-meteo.com/v1/forecast"},
                 cpr::Parameters{
                     {"latitude", city.GetLat()},
                     {"longitude", city.GetLon()},
                     {"hourly", "temperature_2m"},
                     {"hourly", "relativehumidity_2m"},
                     {"hourly", "apparent_temperature"},
                     {"hourly", "weathercode"},
                     {"hourly", "windspeed_10m"},
                     {"start_date", city.GetStartDate()},
                     {"end_date", city.GetEndDate()},
                 });
    if (r.status_code == 200) {
        nlohmann::json response_data = nlohmann::json::parse(r.text);
        // std::cout << r.text;
        city.DataPreprocessing(response_data);
    } else {
        std::cout << r.error.message << "\n";
    }
}

void WeatherForecast::Print() {
    using namespace ftxui;
    auto screen = ScreenInteractive::Fullscreen();

    size_t cur = 0;
    auto renderer = Renderer([&] {
        return vbox(MakeViewCurrentDay(cities[cur]), MakeViewDays(cities[cur]));
    });
    auto component = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Character('q')) {
            screen.ExitLoopClosure()();
            return true;
        }

        if (event == Event::ArrowDown || event == Event::ArrowRight) {
            cur = (cur + 1) % cities.Size();
            std::cout << cities[cur].GetName();
            return true;
        }
        if (event == Event::ArrowUp || event == Event::ArrowLeft) {
            cur = (cur == 0 ? cities.Size() - 1 : cur - 1);
            std::cout << cities[cur].GetName();
            return true;
        }
        return false;
    });
    screen.Loop(component);
}

ftxui::Element WeatherForecast::MakeImageWeather(WeatherCode code) const {
    using namespace ftxui;
    switch (code) {
        case ClearSky:
        case MainlyClear:
            return vbox({text("    \\   /    ") | color(Color::Yellow1),
                         text("     .-.     ") | color(Color::Yellow1),
                         text("  ― (   ) ―  ") | color(Color::Yellow1),
                         text("     `-’     ") | color(Color::Yellow1),
                         text("    /   \\    ") | color(Color::Yellow1)});
        case PartlyCloudy:
            return vbox({text("   \\  /      ") | color(Color::Yellow1),
                         hbox({text(" _ /\"\"") | color(Color::Yellow1),
                               text(".-.    ")}),
                         hbox({text("   \\_") | color(Color::Yellow1),
                               text("(   ).  ")}),
                         hbox({text("   /") | color(Color::Yellow1),
                               text("(___(__) ")}),
                         text("             ")});

        case Overcast:
            return vbox({text("             "),
                         text("     .--.    ") | color(Color::SkyBlue1),
                         text("  .-(    ).  ") | color(Color::SkyBlue1),
                         text(" (___.__)__) ") | color(Color::SkyBlue1),
                         text("             ")});
        case Fog:
        case DepositingRimeFog:
            return vbox({text("             "),
                         text(" _ - _ - _ - ") | color(Color::GrayDark),
                         text("  _ - _ - _  ") | color(Color::GrayDark),
                         text(" _ - _ - _ - ") | color(Color::GrayDark),
                         text("             ")});

        case DrizzleLight:
        case DrizzleModerate:
        case RainSlight:
        case RainModerate:
            return vbox({text("     .-.     ") | color(Color::SkyBlue1),
                         text("    (   ).   ") | color(Color::SkyBlue1),
                         text("   (___(__)  ") | color(Color::SkyBlue1),
                         text("    ‘ ‘ ‘ ‘  "), text("   ‘ ‘ ‘ ‘   ")});
        case DrizzleDense:
        case RainHeavy:
            return vbox({text("     .-.     ") | color(Color::SkyBlue1),
                         text("    (   ).   ") | color(Color::SkyBlue1),
                         text("   (___(__)  ") | color(Color::SkyBlue1),
                         text("  ‚‘‚‘‚‘‚‘   "), text("  ‚’‚’‚’‚’   ")});

        case FreezingDrizzleLight:
        case FreezingRainLight:
            return vbox({hbox({text(" _`/\"\"") | color(Color::Yellow1),
                               text(".-.    ")}),
                         hbox({text("  ,\\_") | color(Color::Yellow1),
                               text("(   ).  ")}),
                         hbox({text("   /") | color(Color::Yellow1),
                               text("(___(__) ")}),
                         text("    ‘ * ‘ *  ") | color(Color::SkyBlue2),
                         text("    * ‘ * ‘  ") | color(Color::SkyBlue2)});

        case FreezingDrizzleDense:
        case FreezingRainHeavy:
            return vbox({text("     .-.     ") | color(Color::SkyBlue1),
                         text("    (   ).   ") | color(Color::SkyBlue1),
                         text("   (___(__)  ") | color(Color::SkyBlue1),
                         text("   ‘ * ‘ *  "), text("   * ‘ * ‘  ")});

        case SnowFallSlight:
        case SnowFallModerate:
            return vbox({text("     .-.     ") | color(Color::SkyBlue1),
                         text("    (   ).   ") | color(Color::SkyBlue1),
                         text("   (___(__)  ") | color(Color::SkyBlue1),
                         text("    *  *  *  "), text("   *  *  *   ")});

        case SnowFallHeavy:
            return vbox({text("     .-.     ") | color(Color::SkyBlue1),
                         text("    (   ).   ") | color(Color::SkyBlue1),
                         text("   (___(__)  ") | color(Color::SkyBlue1),
                         text("   * * * *   "), text("  * * * *    ")});

        case SnowGrains:
            return vbox({text("     .-.     ") | color(Color::SkyBlue1),
                         text("    (   ).   ") | color(Color::SkyBlue1),
                         text("   (___(__)  ") | color(Color::SkyBlue1),
                         text("   • • • •   "), text("  • • • •    ")});

        case RainShowersSlight:
        case RainShowersModerate:
            return vbox({hbox({text(" _`/\"\"") | color(Color::Yellow1),
                               text(".-.    ")}),
                         hbox({text("  ,\\_") | color(Color::Yellow1),
                               text("(   ).  ")}),
                         hbox({text("   /") | color(Color::Yellow1),
                               text("(___(__) ")}),
                         text("     ‘ ‘ ‘ ‘ ") | color(Color::SkyBlue2),
                         text("    ‘ ‘ ‘ ‘  ") | color(Color::SkyBlue2)});

        case RainShowersViolent:
            return vbox({hbox({text(" _`/\"\"") | color(Color::Yellow1),
                               text(".-.    ")}),
                         hbox({text("  ,\\_") | color(Color::Yellow1),
                               text("(   ).  ")}),
                         hbox({text("   /") | color(Color::Yellow1),
                               text("(___(__) ")}),
                         text("   ‚‘‚‘‚‘‚‘  ") | color(Color::SkyBlue2),
                         text("   ‚’‚’‚’‚’  ") | color(Color::SkyBlue2)});

        case SnowShowersSlight:
            return vbox({hbox({text(" _`/\"\"") | color(Color::Yellow1),
                               text(".-.    ")}),
                         hbox({text("  ,\\_") | color(Color::Yellow1),
                               text("(   ).  ")}),
                         hbox({text("   /") | color(Color::Yellow1),
                               text("(___(__) ")}),
                         text("     *  *  * ") | color(Color::SkyBlue2),
                         text("   *  *  *  ") | color(Color::SkyBlue2)});

        case SnowShowersHeavy:
            return vbox({hbox({text(" _`/\"\"") | color(Color::Yellow1),
                               text(".-.    ")}),
                         hbox({text("  ,\\_") | color(Color::Yellow1),
                               text("(   ).  ")}),
                         hbox({text("   /") | color(Color::Yellow1),
                               text("(___(__) ")}),
                         text("    * * * *  ") | color(Color::SkyBlue2),
                         text("   * * * *   ") | color(Color::SkyBlue2)});

        case Thunderstorm:
            vbox({hbox({text(" _`/\"\"") | color(Color::Yellow1),
                        text(".-.    ")}),
                  hbox({text("  ,\\_") | color(Color::Yellow1),
                        text("(   ).  ")}),
                  hbox({text("   /") | color(Color::Yellow1),
                        text("(___(__) ")}),
                  text("   ‚‘⚡‘‚⚡‚‘  ") | color(Color::SkyBlue2),
                  text("  ‚’‚’⚡’‚’   ") | color(Color::SkyBlue2)});

        case ThunderstormWithHailSlight:
        case ThunderstormWithHailHeavy:
            return vbox({hbox({text(" _`/\"\"") | color(Color::Yellow1),
                               text(".-.    ")}),
                         hbox({text("  ,\\_") | color(Color::Yellow1),
                               text("(   ).  ")}),
                         hbox({text("   /") | color(Color::Yellow1),
                               text("(___(__) ")}),
                         text("   * ⚡ * ⚡ * ") | color(Color::SkyBlue2),
                         text("  * ⚡ * ⚡ *  ") | color(Color::SkyBlue2)});

        default:
            return vbox({text("    .-.      "), text("     __)     "),
                         text("    (        "), text("     `-’     "),
                         text("      •      ")});
            ;
    }
}
ftxui::Color WeatherForecast::GetColorNumber(int value) const {
    using namespace ftxui;

    if (value >= 25) return Color::OrangeRed1;
    if (value >= 20) return Color::Orange1;
    if (value >= 15) return Color::GreenLight;
    if (value >= 10) return Color::GreenYellow;
    if (value >= 5) return Color::GreenLight;
    if (value >= 0) return Color::BlueLight;
    if (value >= -10) return Color::SkyBlue1;
    return Color::Blue1;
}

ftxui::Element WeatherForecast::MakeTemperature(float min, float max,
                                                const std::string& unit) const {
    using namespace ftxui;
    int min_temperature = static_cast<int>(roundf(min));
    int max_temperature = static_cast<int>(roundf(max));

    return hbox({text("from "),
                 text(std::to_string(min_temperature)) |
                     color(GetColorNumber(min_temperature)),
                 text(unit), text(" to "),
                 text(std::to_string(max_temperature)) |
                     color(GetColorNumber(max_temperature)),
                 text(unit)});
}
ftxui::Element WeatherForecast::MakeWindspeed(float min, float max,
                                              const std::string& unit) const {
    using namespace ftxui;
    int min_speed = static_cast<int>(roundf(min));
    int max_speed = static_cast<int>(roundf(max));

    return hbox(
        {text(std::to_string(min_speed)) | color(GetColorNumber(min_speed)),
         text("-"),
         text(std::to_string(max_speed)) | color(GetColorNumber(max_speed)),
         text(" " + unit)});
}
ftxui::Element WeatherForecast::MakeRelativehumidity(
    float min, float max, const std::string& unit) const {
    using namespace ftxui;
    int min_relativehumidity = static_cast<int>(roundf(min));
    int max_relativehumidity = static_cast<int>(roundf(max));

    return hbox({text(std::to_string(min_relativehumidity)) |
                     color(GetColorNumber(min_relativehumidity)),
                 text("-"),
                 text(std::to_string(max_relativehumidity)) |
                     color(GetColorNumber(max_relativehumidity)),
                 text(" " + unit)});
}

ftxui::Element WeatherForecast::MakeDescriptionWeather(WeatherCode code) const {
    using namespace ftxui;
    return text(GetDescriptionWeather(code));
}

ftxui::Element WeatherForecast::MakeMorning(const WeatherOfDay& data) const {
    using namespace ftxui;
    auto temperature_2m = data.GetAverages().at("temperature_2m");
    auto windspeed_10m = data.GetAverages().at("windspeed_10m");
    auto relativehumidity_2m = data.GetAverages().at("relativehumidity_2m");

    return vbox({text("Morning") | hcenter, separator(),
                 hbox({MakeImageWeather(data.GetWeaterCodes().morning),
                       vbox({MakeDescriptionWeather(
                                 data.GetWeaterCodes().morning),
                             MakeTemperature(temperature_2m.morning.minimum,
                                             temperature_2m.morning.maximum,
                                             temperature_2m.unit),
                             MakeWindspeed(windspeed_10m.morning.minimum,
                                           windspeed_10m.morning.maximum,
                                           windspeed_10m.unit),
                             MakeRelativehumidity(
                                 relativehumidity_2m.morning.minimum,
                                 relativehumidity_2m.morning.maximum,
                                 relativehumidity_2m.unit)}) |
                           align_right})}) |
           flex_grow;
}

ftxui::Element WeatherForecast::MakeMidday(const WeatherOfDay& data) const {
    using namespace ftxui;
    auto temperature_2m = data.GetAverages().at("temperature_2m");
    auto windspeed_10m = data.GetAverages().at("windspeed_10m");
    auto relativehumidity_2m = data.GetAverages().at("relativehumidity_2m");

    return vbox(
               {text("Midday") | hcenter, separator(),
                hbox(
                    {MakeImageWeather(data.GetWeaterCodes().midday),
                     vbox({MakeDescriptionWeather(data.GetWeaterCodes().midday),
                           MakeTemperature(temperature_2m.midday.minimum,
                                           temperature_2m.midday.maximum,
                                           temperature_2m.unit),
                           MakeWindspeed(windspeed_10m.midday.minimum,
                                         windspeed_10m.midday.maximum,
                                         windspeed_10m.unit),
                           MakeRelativehumidity(
                               relativehumidity_2m.midday.minimum,
                               relativehumidity_2m.midday.maximum,
                               relativehumidity_2m.unit)}) |
                         align_right})}) |
           flex_grow;
}

ftxui::Element WeatherForecast::MakeEvening(const WeatherOfDay& data) const {
    using namespace ftxui;
    auto temperature_2m = data.GetAverages().at("temperature_2m");
    auto windspeed_10m = data.GetAverages().at("windspeed_10m");
    auto relativehumidity_2m = data.GetAverages().at("relativehumidity_2m");

    return vbox({text("Evening") | hcenter, separator(),
                 hbox({MakeImageWeather(data.GetWeaterCodes().evening),
                       vbox({MakeDescriptionWeather(
                                 data.GetWeaterCodes().evening),
                             MakeTemperature(temperature_2m.evening.minimum,
                                             temperature_2m.evening.maximum,
                                             temperature_2m.unit),
                             MakeWindspeed(windspeed_10m.evening.minimum,
                                           windspeed_10m.evening.maximum,
                                           windspeed_10m.unit),
                             MakeRelativehumidity(
                                 relativehumidity_2m.evening.minimum,
                                 relativehumidity_2m.evening.maximum,
                                 relativehumidity_2m.unit)}) |
                           align_right})}) |
           flex_grow;
}

ftxui::Element WeatherForecast::MakeNight(const WeatherOfDay& data) const {
    using namespace ftxui;
    auto temperature_2m = data.GetAverages().at("temperature_2m");
    auto windspeed_10m = data.GetAverages().at("windspeed_10m");
    auto relativehumidity_2m = data.GetAverages().at("relativehumidity_2m");

    return vbox(
               {text("Night") | hcenter, separator(),
                hbox({MakeImageWeather(data.GetWeaterCodes().night),
                      vbox({MakeDescriptionWeather(data.GetWeaterCodes().night),
                            MakeTemperature(temperature_2m.night.minimum,
                                            temperature_2m.night.maximum,
                                            temperature_2m.unit),
                            MakeWindspeed(windspeed_10m.night.minimum,
                                          windspeed_10m.night.maximum,
                                          windspeed_10m.unit),
                            MakeRelativehumidity(
                                relativehumidity_2m.night.minimum,
                                relativehumidity_2m.night.maximum,
                                relativehumidity_2m.unit)}) |
                          align_right})}) |
           flex_grow;
}

ftxui::Element WeatherForecast::MakeViewDays(
    const weather_forecast::City& city) const {
    using namespace ftxui;
    Elements elements;
    auto data = city.GetDays();
    for (int i = 0; i < city.GetCountDay(); ++i) {
        elements.push_back(
            window(text(data[i].GetDate()) | hcenter | bold,
                   hbox({MakeMorning(data[i]), separator(), MakeMidday(data[i]),
                         separator(), MakeEvening(data[i]), separator(),
                         MakeNight(data[i])})));
    }
    return vbox(std::move(elements));
};
ftxui::Element WeatherForecast::MakeViewCurrentDay(
    const weather_forecast::City& city) const {
    using namespace ftxui;
    auto data = city.GetDays();

    auto temperature_2m = data[0].GetAverages().at("temperature_2m");
    auto windspeed_10m = data[0].GetAverages().at("windspeed_10m");
    auto relativehumidity_2m = data[0].GetAverages().at("relativehumidity_2m");
    return vbox(
        {text("Weather report: " + city.GetName()) | bold |
             color(Color::Aquamarine3),
         text(""),
         hbox({MakeImageWeather(data[0].GetWeaterCodes().evening),
               vbox({MakeDescriptionWeather(data[0].GetWeaterCodes().all_day),
                     MakeTemperature(temperature_2m.all_day.minimum,
                                     temperature_2m.all_day.maximum,
                                     temperature_2m.unit),
                     MakeWindspeed(windspeed_10m.all_day.minimum,
                                   windspeed_10m.all_day.maximum,
                                   windspeed_10m.unit),
                     MakeRelativehumidity(relativehumidity_2m.all_day.minimum,
                                          relativehumidity_2m.all_day.maximum,
                                          relativehumidity_2m.unit)})}) |
             flex,
         text("")});
}
};  // namespace weather_forecast