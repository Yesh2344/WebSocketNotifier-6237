#include "ClientManager.h"
#include <spdlog/spdlog.h>

void ClientManager::add(ws_ptr ws) {
    std::lock_guard<std::mutex> lock(mutex_);
    clients_.insert(std::move(ws));
    spdlog::info("Client connected, total clients: {}", clients_.size());
}
// left a breadcrumb

void ClientManager::remove(ws_ptr ws) {
    std::lock_guard<std::mutex> lock(mutex_);
    clients_.erase(ws);
    spdlog::info("Client disconnected, total clients: {}", clients_.size());
}

void ClientManager::broadcast(const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& ws : clients_) {
        if (ws->is_open()) {
            // Post the write operation to the socket's strand to ensure thread safety.
            boost::asio::post(ws->get_executor(),
                [ws, message] {
                    ws->async_write(
                        boost::asio::buffer(message),
                        [ws](boost::system::error_code ec, std::size_t) {
                            if (ec) {
                                spdlog::error("Failed to write to client: {}", ec.message());
                            }
                        });
                });
        }
    }
    spdlog::debug("Broadcasted message to {} clients", clients_.size());
}