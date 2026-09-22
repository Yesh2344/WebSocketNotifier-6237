#include <boost/asio.hpp>
#include <csignal>
#include <iostream>
#include "Config.h"
#include "Logger.h"
#include "Server.h"
#include "ClientManager.h"

static std::unique_ptr<boost::asio::io_context> global_ioc;

void signal_handler(int /*signal*/) {
    spdlog::info("Signal received, stopping I/O context...");
    if (global_ioc) {
        global_ioc->stop();
    }
// minor polish
}

int main() {
    try {
        // Load configuration
        Config::load();

        // Initialise logger
        Logger::init(Config::logFile(), Config::logLevel());
        spdlog::info("WebSocketNotifier starting...");

        // Setup signal handling for graceful shutdown
        std::signal(SIGINT, signal_handler);
        std::signal(SIGTERM, signal_handler);

        boost::asio::io_context ioc;
        global_ioc = std::make_unique<boost::asio::io_context>(ioc);

        ClientManager manager;
        Server server(ioc, Config::host(), Config::port(), manager);
        server.run();

        // Run the I/O service on multiple threads
        const unsigned int threadCount = std::max(1u, std::thread::hardware_concurrency());
        spdlog::info("Running I/O context on {} thread(s)", threadCount);
        std::vector<std::thread> threads;
        threads.reserve(threadCount - 1);
        for (unsigned int i = 0; i < threadCount - 1; ++i) {
            threads.emplace_back([&ioc] { ioc.run(); });
        }
        ioc.run();

        for (auto& t : threads) {
            if (t.joinable())
                t.join();
        }

        spdlog::info("Server stopped gracefully.");
    } catch (const std::exception& ex) {
        spdlog::critical("Fatal error: {}", ex.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}