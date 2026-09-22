#include <gtest/gtest.h>
#include <boost/asio.hpp>
#include <thread>
#include "Server.h"
#include "ClientManager.h"
#include "Config.h"
#include "Logger.h"

class WebSocketServerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Use a test config (in-memory)
        Config::load("config.json"); // Assume config.json points to localhost:0 for random port
        Logger::init("logs/test.log", "debug");

        ioc_ = std::make_unique<boost::asio::io_context>();
        manager_ = std::make_unique<ClientManager>();

        // Choose a free port by binding to port 0
        boost::asio::ip::tcp::acceptor tempAcceptor(*ioc_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 0));
        auto endpoint = tempAcceptor.local_endpoint();
        tempAcceptor.close();

        server_ = std::make_unique<Server>(*ioc_, endpoint.address().to_string(),
                                          endpoint.port(), *manager_);
        server_->run();

        // Run I/O in background thread
        ioThread_ = std::thread([this] { ioc_->run(); });
    }

    void TearDown() override {
        ioc_->stop();
        if (ioThread_.joinable())
            ioThread_.join();
    }

    std::unique_ptr<boost::asio::io_context> ioc_;
    std::unique_ptr<Server> server_;
    std::unique_ptr<ClientManager> manager_;
    std::thread ioThread_;
};

TEST_F(WebSocketServerTest, BroadcastMessage) {
    // Connect a client using Boost.Beast
    boost::asio::ip::tcp::resolver resolver(*ioc_);
    auto const results = resolver.resolve("127.0.0.1",
        std::to_string(server_->acceptor_.local_endpoint().port()));

    boost::beast::websocket::stream<boost::asio::ip::tcp::socket> ws(*ioc_);
    boost::asio::connect(ws.next_layer(), results);
    ws.handshake("127.0.0.1", "/");

    // Send a message
    std::string payload = R"({"type":"test","msg":"hello"})";
    ws.write(boost::asio::buffer(payload));

    // Receive broadcast (should be the same message)
    boost::beast::flat_buffer buffer;
    ws.read(buffer);
    std::string received = boost::beast::buffers_to_string(buffer.data());

    EXPECT_EQ(received, payload);
}