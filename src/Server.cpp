#include "Server.h"
#include <spdlog/spdlog.h>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <memory>

using boost::asio::ip::tcp;
namespace beast = boost::beast;
namespace websocket = beast::websocket;

Server::Server(boost::asio::io_context& ioc,
               const std::string& address,
               uint16_t port,
               ClientManager& manager)
    : ioc_(ioc),
      acceptor_(ioc),
      manager_(manager) {
    boost::system::error_code ec;

    // Resolve address
    tcp::resolver resolver(ioc_);
    auto endpoint = *resolver.resolve(address, std::to_string(port)).begin();

    // Open acceptor
    acceptor_.open(endpoint.protocol(), ec);
    if (ec) {
        throw std::runtime_error("Failed to open acceptor: " + ec.message());
    }

    // Allow address reuse
    acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);
    if (ec) {
        throw std::runtime_error("Failed to set socket option: " + ec.message());
    }

    // Bind to the server address
    acceptor_.bind(endpoint, ec);
    if (ec) {
        throw std::runtime_error("Failed to bind: " + ec.message());
    }

    // Start listening
    acceptor_.listen(boost::asio::socket_base::max_listen_connections, ec);
    if (ec) {
        throw std::runtime_error("Failed to listen: " + ec.message());
    }
}

void Server::run() {
    spdlog::info("Server listening on {}:{}", acceptor_.local_endpoint().address().to_string(),
                 acceptor_.local_endpoint().port());
    doAccept();
}

void Server::doAccept() {
    acceptor_.async_accept(
        boost::asio::make_strand(ioc_),
        std::bind(&Server::onAccept, this,
                  std::placeholders::_1,
                  std::placeholders::_2));
}

void Server::onAccept(boost::system::error_code ec, tcp::socket socket) {
    if (ec) {
        spdlog::error("Accept error: {}", ec.message());
    } else {
        // Create a shared pointer to the websocket stream
        auto ws = std::make_shared<websocket::stream<tcp::socket>>(std::move(socket));

        // Set timeout options (optional)
        ws->set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));

        // Accept the WebSocket handshake
        ws->async_accept(
            [this, ws](boost::system::error_code ec) {
                if (ec) {
                    spdlog::error("WebSocket handshake failed: {}", ec.message());
                    return;
                }

                manager_.add(ws);

                // Start reading messages
                auto readBuffer = std::make_shared<beast::flat_buffer>();
                std::function<void()> doRead;
                doRead = [this, ws, readBuffer, &doRead]() mutable {
                    ws->async_read(*readBuffer,
                        [this, ws, readBuffer, &doRead](boost::system::error_code ec, std::size_t bytes_transferred) mutable {
                            if (ec) {
                                if (ec != websocket::error::closed) {
                                    spdlog::error("Read error: {}", ec.message());
                                }
                                manager_.remove(ws);
                                return;
                            }

                            // Extract the message as a string
                            std::string msg = beast::buffers_to_string(readBuffer->data());
                            readBuffer->consume(readBuffer->size()); // clear buffer

                            spdlog::debug("Received message ({} bytes): {}", bytes_transferred, msg);
                            manager_.broadcast(msg); // Echo to all clients

                            // Continue reading
                            doRead();
                        });
                };
                doRead();
            });
    }

    // Accept another connection
    doAccept();
}