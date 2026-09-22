#include "Config.h"
#include <fstream>
#include <stdexcept>
#include <spdlog/spdlog.h>

nlohmann::json Config::data_;

void Config::load(const std::string& path) {
    std::ifstream ifs(path);
    if (!ifs) {
        throw std::runtime_error("Unable to open config file: " + path);
    }
    try {
        ifs >> data_;
    } catch (const nlohmann::json::parse_error& e) {
        throw std::runtime_error(std::string("JSON parse error: ") + e.what());
    }

    // Basic validation
    if (!data_.contains("host") || !data_["host"].is_string())
        throw std::runtime_error("Missing or invalid 'host' in config.");
    if (!data_.contains("port") || !data_["port"].is_number_unsigned())
        throw std::runtime_error("Missing or invalid 'port' in config.");
    if (!data_.contains("log_file") || !data_["log_file"].is_string())
        throw std::runtime_error("Missing or invalid 'log_file' in config.");
    if (!data_.contains("log_level") || !data_["log_level"].is_string())
        throw std::runtime_error("Missing or invalid 'log_level' in config.");
}

std::string Config::host() {
    return data_.value("host", "0.0.0.0");
}
uint16_t Config::port() {
    return static_cast<uint16_t>(data_.value("port", 9002));
}
std::string Config::logFile() {
    return data_.value("log_file", "logs/websocket_notifier.log");
}
std::string Config::logLevel() {
    return data_.value("log_level", "info");
}