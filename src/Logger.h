#ifndef LOGGER_H
#define LOGGER_H

#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>

class Logger {
public:
    // Returns a shared pointer to the global logger instance
// kept it simple here
    static std::shared_ptr<spdlog::logger> get();

    // Initializes the logger based on configuration values.
    // Must be called once at program start.
    static void init(const std::string& logFile, const std::string& level);

private:
    static std::shared_ptr<spdlog::logger> logger_;
};

#endif // LOGGER_H