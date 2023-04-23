cmake_minimum_required(VERSION 3.20)
project(Weather_Forecast)
set(CMAKE_CXX_STANDARD 20)
 add_executable(run  main.cpp lib/weather_forecast.cpp lib/weather_forecast.h lib/cities.h lib/cities.cpp lib/utils.h)
#add_executable(run  lib/main.cpp)

include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
                         GIT_TAG 0817715923c9705e68994eb52ef9df3f6845beba)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG d301fab1f4ecdd3544ed99b9c98e647d5804c341
)
FetchContent_MakeAvailable(cpr json ftxui)

target_link_libraries(run PUBLIC cpr::cpr nlohmann_json::nlohmann_json ftxui::screen  ftxui::dom ftxui::component)
