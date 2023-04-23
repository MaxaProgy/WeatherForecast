#pragma once
#include <math.h>

#include <list>
#include <nlohmann/json.hpp>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>

#include "cities.h"
#include "ftxui/dom/elements.hpp"

namespace weather_forecast {

class WeatherForecast {
   private:
    Cities cities;
    std::mutex cities_mutex;
    bool terminate;
    bool reload;
    void Print();
    ftxui::Color GetColorNumber(int value) const;

    ftxui::Element MakeImageWeather(WeatherCode code) const;
    ftxui::Element MakeTemperature(float min, float max,
                                   const std::string& unit) const;
    ftxui::Element MakeWindspeed(float min, float max,
                                 const std::string& unit) const;
    ftxui::Element MakeRelativehumidity(float min, float max,
                                        const std::string& unit) const;
    ftxui::Element MakeDescriptionWeather(WeatherCode code) const;
    ftxui::Element MakeMorning(const WeatherOfDay& data) const;
    ftxui::Element MakeMidday(const WeatherOfDay& data) const;
    ftxui::Element MakeEvening(const WeatherOfDay& data) const;
    ftxui::Element MakeNight(const WeatherOfDay& data) const;
    ftxui::Element MakeViewCurrentDay(const weather_forecast::City& city) const;
    ftxui::Element MakeViewDays(const weather_forecast::City& city) const;
    std::thread thread_frequency;

   public:
    WeatherForecast(const std::string& path_to_config);
    ~WeatherForecast();
};

};  // namespace weather_forecast