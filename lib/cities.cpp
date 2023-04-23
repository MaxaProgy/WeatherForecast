#pragma once
#include "cities.h"

#include <cpr/cpr.h>

#include <ctime>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "utils.h"

namespace weather_forecast {
static const char* CITY_API =
    "lxusuyt46FHdzLQ3qYpQxQ==eMA6YTo13WCftM2Y";  // std::getenv("CITY_API");

const WeatherCode GetWeaterCode(const int num) {
    switch (num) {
        case 0:
            return ClearSky;
        case 1:
            return MainlyClear;
        case 2:
            return PartlyCloudy;
        case 3:
            return Overcast;
        case 45:
            return Fog;
        case 48:
            return DepositingRimeFog;
        case 51:
            return DrizzleLight;
        case 53:
            return DrizzleModerate;
        case 55:
            return DrizzleDense;
        case 56:
            return FreezingDrizzleLight;
        case 57:
            return FreezingDrizzleDense;
        case 61:
            return RainSlight;
        case 63:
            return RainModerate;
        case 65:
            return RainHeavy;
        case 66:
            return FreezingRainLight;
        case 67:
            return FreezingRainHeavy;
        case 71:
            return SnowFallSlight;
        case 73:
            return SnowFallModerate;
        case 75:
            return SnowFallHeavy;
        case 77:
            return SnowGrains;
        case 80:
            return RainShowersSlight;
        case 81:
            return RainShowersModerate;
        case 82:
            return RainShowersViolent;
        case 85:
            return SnowShowersSlight;
        case 86:
            return SnowShowersHeavy;
        case 95:
            return Thunderstorm;
        case 96:
            return ThunderstormWithHailSlight;
        case 99:
            return ThunderstormWithHailHeavy;
        default:
            return None;
    }
}

const std::string GetDescriptionWeather(const WeatherCode code) {
    switch (code) {
        case ClearSky:
            return "Clear sky";
        case MainlyClear:
            return "Mainly clear";
        case PartlyCloudy:
            return "Partly cloudy";
        case Overcast:
            return "Overcast";
        case Fog:
            return "Fog";
        case DepositingRimeFog:
            return "Depositing rime fog";
        case DrizzleLight:
            return "Drizzle light";
        case DrizzleModerate:
            return "Drizzle moderate";
        case DrizzleDense:
            return "Drizzle dense";
        case FreezingDrizzleLight:
            return "Freezing drizzle light";
        case FreezingDrizzleDense:
            return "Freezing drizzle dense";
        case RainSlight:
            return "Rain slight";
        case RainModerate:
            return "Rain moderate";
        case RainHeavy:
            return "Rain heavy";
        case FreezingRainLight:
            return "Freezing rain light";
        case FreezingRainHeavy:
            return "Freezing rain heavy";
        case SnowFallSlight:
            return "Snow fall slight";
        case SnowFallModerate:
            return "Snow fall moderate";
        case SnowFallHeavy:
            return "Snow fall heavy";
        case SnowGrains:
            return "Snow grains";
        case RainShowersSlight:
            return "Rain showers slight";
        case RainShowersModerate:
            return "Rain showers moderate";
        case RainShowersViolent:
            return "Rain showers violent";
        case SnowShowersSlight:
            return "Snow showers slight";
        case SnowShowersHeavy:
            return "Snow showers heavy";
        case Thunderstorm:
            return "Thunderstorm";
        case ThunderstormWithHailSlight:
            return "Thunderstorm with hail slight";
        case ThunderstormWithHailHeavy:
            return "Thunderstorm with hail heavy";
        case None:
            return "";
    }
}

// ======================= WeatherOfDay =========================

void WeatherOfDay::InitHourlyParameters() {
    measurement["temperature_2m"];
    measurement["relativehumidity_2m"];
    measurement["apparent_temperature"];
    measurement["windspeed_10m"];
};

void WeatherOfDay::SetDateAverage(size_t day) {
    std::time_t then = std::time({}) + day * 24 * 3600;

    date.resize(11);
    strftime(&date[0], 11, "%F", std::localtime(&then));
    date.erase(date.find_first_of('\0'));
}

WeatherOfDay::WeatherOfDay() { InitHourlyParameters(); }

bool WeatherOfDay::KeyMeasurementContains(const std::string& key) const {
    return measurement.contains(key);
}

template <class InputIt,
          class T = typename std::iterator_traits<InputIt>::value_type>
void WeatherOfDay::SetAverage(const std::string& key, InputIt begin,
                              InputIt end) {
    measurement[key].all_day.minimum = *std::min_element(begin, end);
    measurement[key].all_day.maximum = *std::max_element(begin, end);

    measurement[key].night.minimum = *std::min_element(begin, begin + 6);
    measurement[key].night.maximum = *std::max_element(begin, begin + 6);

    measurement[key].morning.minimum = *std::min_element(begin + 6, begin + 12);
    measurement[key].morning.maximum = *std::max_element(begin + 6, begin + 12);

    measurement[key].midday.minimum = *std::min_element(begin + 12, begin + 18);
    measurement[key].midday.maximum = *std::max_element(begin + 12, begin + 18);

    measurement[key].evening.minimum = *std::min_element(begin + 18, end);
    measurement[key].evening.maximum = *std::max_element(begin + 18, end);
}

template <class InputIt,
          class T = typename std::iterator_traits<InputIt>::value_type>
void WeatherOfDay::SetWeatherCodes(InputIt begin, InputIt end) {
    std::cout << MostCommon(begin, end);
    weathercode.all_day = GetWeaterCode(MostCommon(begin, end));
    weathercode.night = GetWeaterCode(MostCommon(begin, begin + 6));
    weathercode.morning = GetWeaterCode(MostCommon(begin + 6, begin + 12));
    weathercode.midday = GetWeaterCode(MostCommon(begin + 12, begin + 18));
    weathercode.evening = GetWeaterCode(MostCommon(begin + 18, end));
}

void WeatherOfDay::SetUnit(const std::string& key, const std::string& unit) {
    measurement[key].unit = unit;
}

const std::string WeatherOfDay::GetDate() const { return date; }

const WeatherOfDay::WeatherCodeForDay& WeatherOfDay::GetWeaterCodes() const {
    return weathercode;
}

const std::unordered_map<std::string, WeatherOfDay::AverageMeasurements>&
WeatherOfDay::GetAverages() const {
    return measurement;
}

// ====================== City =========================

City::City(const std::string& name, int count_day)
    : name(name), count_day(count_day) {
    UpdateForecastTimeInterval();
    SetCoordinate();
    weather_data.resize(count_day);
}

void City::DataPreprocessing(nlohmann::json data) {
    for (const auto& [key, value] : data["hourly_units"].items()) {
        if (!weather_data[0].KeyMeasurementContains(key)) continue;
        for (int i = 0; i < count_day; ++i) {
            auto data = value.get<std::string>();
            weather_data[i].SetUnit(key, value);
        }
    }

    for (const auto& [key, values] : data["hourly"].items()) {
        if (!weather_data[0].KeyMeasurementContains(key) &&
            key != "weathercode") {
            std::cout << key;

            continue;
        }

        auto data = values.get<std::vector<float>>();
        for (int i = 0; i < count_day; ++i) {
            weather_data[i].SetDateAverage(i + 1);
            auto start_day = data.begin() + i * 24;
            auto end_day = data.begin() + (i + 1) * 24;

            if (key == "weathercode") {
                std::cout << 9999;
                weather_data[i].SetWeatherCodes(start_day, end_day);

            } else {
                weather_data[i].SetAverage(key, start_day, end_day);
            }
        }
    }
}
const std::string& City::GetName() const { return name; }

const int City::GetCountDay() const { return count_day; }

void City::SetCoordinate() {
    cpr::Response r = cpr::Get(cpr::Url{"https://api.api-ninjas.com/v1/city"},
                               cpr::Header{{"X-Api-Key", CITY_API}},
                               cpr::Parameters{
                                   {"name", name},
                               });
    if (r.status_code == 200) {
        nlohmann::json response_data = nlohmann::json::parse(r.text);
        lat = response_data[0]["latitude"].dump();
        lon = response_data[0]["longitude"].dump();

    } else {
        // TODO: обработка ошибок
    }
}
const std::string& City::GetLat() const { return lat; }
const std::string& City::GetLon() const { return lon; }

void City::UpdateForecastTimeInterval() {
    std::time_t now = std::time({});
    std::time_t then = now + count_day * 24 * 3600;

    start_date.resize(11);
    end_date.resize(11);
    strftime(&start_date[0], 11, "%F", std::localtime(&now));
    strftime(&end_date[0], 11, "%F", std::localtime(&then));
    start_date.erase(start_date.find_first_of('\0'));
    end_date.erase(end_date.find_first_of('\0'));
}
const std::string& City::GetStartDate() const { return start_date; }
const std::string& City::GetEndDate() const { return end_date; }

const std::vector<WeatherOfDay>& City::GetDays() const { return weather_data; }

// ======================= Cities =========================
void Cities::AddCity(const std::string& name, int count_day) {
    cities.emplace_back(name, count_day);
}
City& Cities::operator[](size_t i) {
    auto iter = cities.begin();
    std::advance(iter, i);
    return *iter;
}

City& Cities::Back() { return cities.back(); }
size_t Cities::Size() const { return cities.size(); }

};  // namespace weather_forecast