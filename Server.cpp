#include "Server.h"

Server::Server(boost::asio::io_context &context, unsigned short port)
    : acceptor_(context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
        acceptor_thread_(&Server::acceptConnection, this){}

void Server::stop(){
    acceptor_.close();
    acceptor_thread_.join();
}

std::set<ClientConnection::Pointer> Server::getClientConnections() {
    std::lock_guard<std::mutex> lock(connections_mutex_);
    return active_connections_;
}

void Server::on_session_started(ClientConnection::Pointer connection) {
    std::lock_guard<std::mutex> lock(connections_mutex_);
    active_connections_.insert(connection);
}

void Server::on_session_stopped(ClientConnection::Pointer connection, const boost::asio::ip::tcp::endpoint& endpoint) {
    std::lock_guard<std::mutex> lock(connections_mutex_);
    std::cout << "Client Disconnected: " << endpoint << std::endl;
    active_connections_.erase(connection);
}

void Server::acceptConnection() {
    while (!acceptor_.is_open()) {
        // Wait for the acceptor to open.
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    while (acceptor_.is_open()) {
        boost::system::error_code ec;

        ClientConnection::Pointer new_session = ClientConnection::create(static_cast<boost::asio::io_context&>(acceptor_.get_executor().context()), *this);

        auto& socket = new_session->socket();
        acceptor_.accept(socket, ec);

        if (!ec) {
            new_session->setRemoteEndpoint(socket.remote_endpoint());
            std::cout << "Connection Accepted: " << new_session->socket().remote_endpoint() << std::endl;
            on_session_started(new_session);
            std::thread(&ClientConnection::start, new_session).detach();
        }
    }
}


/*
void Server::acceptConnection() {
    while (!acceptor_.is_open()) {
        // Wait for the acceptor to open.
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    while (acceptor_.is_open()) {
        boost::system::error_code ec;

        ClientConnection::Pointer new_session = ClientConnection::create(static_cast<boost::asio::io_context&>(acceptor_.get_executor().context()), *this);

        auto& socket = new_session->socket();
        acceptor_.accept(socket, ec);
        if (!ec) {
            std::cout << "Connection Accepted: " << new_session->socket().remote_endpoint() << std::endl;
            on_session_started(new_session);
            std::thread(&ClientConnection::start, new_session).detach();
        }
    }
}
*/
