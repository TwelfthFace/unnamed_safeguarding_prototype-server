#pragma once

#include <boost/asio.hpp>
#include <array>
#include <vector>
#include <deque>
#include <tuple>
#include <string>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>

#include "Header.h"
#include "AckHeader.h"
#include "clientpreviewwidget.h"

class Server;
class ClientPreviewWidget;

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
    void lockScreen();
    void unlockScreen();
    void setRemoteEndpoint(const boost::asio::ip::tcp::endpoint& endpoint);
    ClientPreviewWidget* preview_ui = nullptr;
    cv::Mat uncompressed_screenshot_data;
    bool isLocked;
    std::vector<u_char>* keyStrokes;
    boost::asio::ip::tcp::endpoint remote_endpoint_;
    virtual ~ClientConnection();
private:
    ClientConnection(boost::asio::io_context& context, Server& server);

    void readHeader();
    void readTextData();
    void readScreenshotData();
    void disconnect();
    void handle_timeout(const boost::system::error_code& error);
    void addToQueue(const Header& header, const std::array<char, 1024>& message);


    Server& server_;
    boost::asio::ip::tcp::socket socket_;
    std::array<char, 1024> text_buffer_;
    std::vector<u_char> screenshot_data_;
    std::deque<std::tuple<Header, std::array<char, 1024>>> msg_queue_;
    std::mutex queue_mutex_;
    Header header_;
};
