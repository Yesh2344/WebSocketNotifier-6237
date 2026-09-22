#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <mutex>
#include <set>
#include <string>

/**
 * @brief Manages active WebSocket sessions.
 *
 * Provides thread‑safe broadcast capability.
 */
class ClientManager {
public:
    using ws_ptr = std::shared_ptr<boost::beast::websocket::stream<
        boost::asio::ip::tcp::socket>>;

    // Register a new session
    void add(ws_ptr ws);

    // Remove a session (called when a session ends)
    void remove(ws_ptr ws);

    // Broadcast a text message to all connected clients
    void broadcast(const std::string& message);

private:
    std::set<ws_ptr> clients_;
    std::mutex mutex_;
};

#endif // CLIENTMANAGER_H