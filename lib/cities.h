#pragma once
#include <list>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace weather_forecast {

enum WeatherCode {
    None,
    ClearSky,                    // 0
    MainlyClear,                 // 1
    PartlyCloudy,                // 2
    Overcast,                    // 3
    Fog,                         // 45
    DepositingRimeFog,           // 48
    DrizzleLight,                // 51
    DrizzleModerate,             // 53
    DrizzleDense,                // 55
    FreezingDrizzleLight,        // 56
    FreezingDrizzleDense,        // 57
    RainSlight,                  // 61
    RainModerate,                // 63
    RainHeavy,                   // 65
    FreezingRainLight,           // 66
    FreezingRainHeavy,           // 67
    SnowFallSlight,              // 71
    SnowFallModerate,            // 73
    SnowFallHeavy,               // 75
    SnowGrains,                  // 77
    RainShowersSlight,           // 80
    RainShowersModerate,         // 81
    RainShowersViolent,          // 82
    SnowShowersSlight,           // 85
    SnowShowersHeavy,            // 86
    Thunderstorm,                // 95
    ThunderstormWithHailSlight,  // 96
    ThunderstormWithHailHeavy    // 99
};
const std::string GetDescriptionWeather(const WeatherCode code);
const WeatherCode GetWeaterCode(const int num);

class City;
class WeatherOfDay {
    friend City;

   private:
    void InitHourlyParameters();

    struct InfoAverage {
        float minimum;
        float maximum;
    };

    struct AverageMeasurements {
        InfoAverage all_day;
        InfoAverage night;
        InfoAverage evening;
        InfoAverage midday;
        InfoAverage morning;
        std::string unit;
    };

    struct WeatherCodeForDay {
        WeatherCode all_day;
        WeatherCode night;
        WeatherCode evening;
        WeatherCode midday;
        WeatherCode morning;
    };
    template <class InputIt,
              class T = typename std::iterator_traits<InputIt>::value_type>
    void SetAverage(const std::string& key, InputIt begin, InputIt end);

    template <class InputIt,
              class T = typename std::iterator_traits<InputIt>::value_type>
    void SetWeatherCodes(InputIt begin, InputIt end);

    void SetUnit(const std::string& key, const std::string& unit);
    void SetDateAverage(size_t day);

    std::unordered_map<std::string, AverageMeasurements> measurement;
    WeatherCodeForDay weathercode;
    std::string date;

   public:
    WeatherOfDay();

    bool KeyMeasurementContains(const std::string& key) const;
    const std::string GetDate() const;
    const WeatherCodeForDay& GetWeaterCodes() const;
    const std::unordered_map<std::string, AverageMeasurements>& GetAverages()
        const;
};

class City {
   private:
    class WeatherAllDays {
       private:
        std::list<WeatherOfDay> data;

       public:
        WeatherOfDay& operator[](size_t i);
        void Pop();
        void AddEmpty();
        void Resize(size_t n);
    };

   public:
    City(const std::string& name, size_t count_day);

    const std::string& GetName() const;
    const size_t GetCountDay() const;

    const WeatherAllDays& GetDays() const;
    void IncrementDaysOfForecast();
    void DecrementDaysOfForecast();
    void UpdateAllForecast();

   private:
    std::string name;
    size_t count_day;

    std::string start_date;
    std::string end_date;

    std::string lat;
    std::string lon;

    WeatherAllDays weather_data;

    void SetCoordinate();
    void UpdateForecastTimeInterval();
    void AddForecastForLastDay();
    void DataPreprocessing(const size_t day, nlohmann::json data);
};

class Cities {
   public:
    void AddCity(const std::string& name, size_t count_day);
    City& Back();
    size_t Size() const;
    City& operator[](size_t i);

   private:
    std::list<City> cities;
};

};  // namespace weather_forecast