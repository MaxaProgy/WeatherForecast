// for text, vbox, window, Element, Elements
#include "lib/weather_forecast.h"

std::string GetPathToConfig(int argc, char** argv) {
    std::string file_path;
    if (argc != 2) {
        throw std::range_error{
            "Invalid input format: pass one line - the path latitudeto the "
            "configuration file."};
    }

    file_path = argv[1];

    if (std::string_view(file_path.end() - 5, file_path.end()) != ".json") {
        throw std::invalid_argument{"File \"" + file_path + "\" is`not json."};
    }
    return argv[1];
}

int main(int argc, char** argv) {
    auto wf = weather_forecast::WeatherForecast(GetPathToConfig(argc, argv));

    return 0;
}