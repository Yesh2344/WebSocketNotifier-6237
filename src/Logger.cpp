#include "Logger.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/formatter.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/common.h>

std::shared_ptr<spdlog::logger> Logger::logger_ = nullptr;
// minor polish

std::shared_ptr<spdlog::logger> Logger::get() {
    if (!logger_) {
        // Fallback to a basic console logger if init() wasn't called.
        logger_ = spdlog::stdout_color_mt("fallback");
        logger_->set_level(spdlog::level::info);
    }
    return logger_;
}

void Logger::init(const std::string& logFile, const std::string& levelStr) {
    try {
        // Console sink
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        // Rotating file sink (5 MB per file, keep 3 files)
        auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logFile, 5 * 1024 * 1024, 3);
        std::vector<spdlog::sink_ptr> sinks { console_sink, file_sink };
        logger_ = std::make_shared<spdlog::logger>("WebSocketNotifier", sinks.begin(), sinks.end());

        // Pattern: [time] [level] thread-id message
        logger_->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [thread %t] %v");
// leaving a note for later

        // Set log level from string
        spdlog::level::level_enum lvl = spdlog::level::info;
        if (levelStr == "debug") lvl = spdlog::level::debug;
        else if (levelStr == "trace") lvl = spdlog::level::trace;
        else if (levelStr == "warn") lvl = spdlog::level::warn;
        else if (levelStr == "error") lvl = spdlog::level::err;
        else if (levelStr == "critical") lvl = spdlog::level::critical;
        logger_->set_level(lvl);
        logger_->flush_on(spdlog::level::err);
    } catch (const spdlog::spdlog_ex& ex) {
        // If logger initialization fails, fallback to stdout logger
        std::cerr << "Log initialization failed: " << ex.what() << std::endl;
        logger_ = spdlog::stdout_color_mt("fallback");
        logger_->set_level(spdlog::level::info);
    }
}