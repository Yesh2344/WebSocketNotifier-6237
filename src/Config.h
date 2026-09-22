#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <nlohmann/json.hpp>

/**
 * @brief Singleton configuration loader.
 *
 * Reads `config.json` (or a custom path) and exposes strongly‑typed getters.
 */
class Config {
public:
    // Load configuration from file. Throws std::runtime_error on failure.
    static void load(const std::string& path = "config.json");

    // Accessors
    static std::string host();
    static uint16_t port();
    static std::string logFile();
    static std::string logLevel();

private:
    Config() = delete;
    static nlohmann::json data_;
};

#endif // CONFIG_H