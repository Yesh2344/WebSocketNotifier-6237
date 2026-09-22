#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
#include <boost/beast/websocket.hpp>
#include "ClientManager.h"

/**
 * @brief Asynchronous WebSocket server.
 *
 * Listens for incoming connections, upgrades to WebSocket, and
 * forwards received messages to the ClientManager for broadcasting.
 */
class Server {
public:
    Server(boost::asio::io_context& ioc,
           const std::string& address,
           uint16_t port,
           ClientManager& manager);

    // Start accepting connections
    void run();

private:
    void doAccept();
    void onAccept(boost::system::error_code ec,
                  boost::asio::ip::tcp::socket socket);

    boost::asio::io_context& ioc_;
    boost::asio::ip::tcp::acceptor acceptor_;
    ClientManager& manager_;
};

#endif // SERVER_H