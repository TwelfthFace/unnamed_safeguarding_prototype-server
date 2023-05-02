#pragma once

#include <boost/asio.hpp>
#include <array>
#include <vector>
#include <deque>
#include <tuple>
#include <string>
#include <iostream>

#include "Header.h"
#include "AckHeader.h"

class Server;

class ClientConnection : public std::enable_shared_from_this<ClientConnection> {
public:
    boost::asio::ip::tcp::socket& socket();
    typedef std::shared_ptr<ClientConnection> Pointer;
    static Pointer create(boost::asio::io_context &context, Server& server);
    void start();
    bool isConnected();
    void stop();
    void requestScreenshot();
    void sendText(const std::string& text);
    void processClient();
    void checkQueue();
    void setRemoteEndpoint(const boost::asio::ip::tcp::endpoint& endpoint);

    virtual ~ClientConnection();

private:
    ClientConnection(boost::asio::io_context& context, Server& server);

    void readHeader();
    void readTextData();
    void readScreenshotData();
    void disconnect();
    void handle_timeout(const boost::system::error_code& error);

    Server& server_;
    boost::asio::ip::tcp::socket socket_;
    std::array<char, 1024> text_buffer_;
    std::vector<u_char> screenshot_data_;
    std::deque<std::tuple<Header, std::array<char, 1024>>> msg_queue_;
    Header header_;
    boost::asio::ip::tcp::endpoint remote_endpoint_;
};
