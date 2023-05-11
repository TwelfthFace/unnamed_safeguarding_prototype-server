#include "Server.h"

Server::Server(unsigned short port)
    : acceptor_(*io_context_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
      ssl_context_(boost::asio::ssl::context::tlsv12),
      acceptor_thread_(&Server::acceptConnection, this),
      io_context_(new boost::asio::io_context()),
      work_guard_(new boost::asio::executor_work_guard<boost::asio::io_context::executor_type>(boost::asio::make_work_guard(*io_context_))),
      io_context_thread_([this]() { io_context_->run(); })
{
    try {
        // Path to your private key
        ssl_context_.use_private_key_file("server.key", boost::asio::ssl::context::pem);

        // Path to your certificate
        ssl_context_.use_certificate_chain_file("server.crt");

        // Path to your CA certificate
        ssl_context_.load_verify_file("ca.crt");

        ssl_context_.set_verify_mode(
            boost::asio::ssl::verify_peer
            | boost::asio::ssl::verify_fail_if_no_peer_cert);
        //SSL_CTX_set_session_cache_mode(ssl_context_.native_handle(), SSL_SESS_CACHE_OFF);

        // Then you set the callback on your context
        //SSL_CTX_set_info_callback(ssl_context_.native_handle(), Server::debug_callback);
    } catch (const boost::system::system_error& ex) {
        std::cerr << "SSL context initialization failed: " << ex.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Server::debug_callback(const SSL *s, int where, int ret) {
    const char *str;
    int w;

    w = where & ~SSL_ST_MASK;

    if (w & SSL_ST_CONNECT) str = "SSL_connect";
    else if (w & SSL_ST_ACCEPT) str = "SSL_accept";
    else str = "undefined";

    if (where & SSL_CB_LOOP) {
        printf("%s:%s\n", str, SSL_state_string_long(s));
    } else if (where & SSL_CB_ALERT) {
        str = (where & SSL_CB_READ) ? "read" : "write";
        printf("SSL3 alert %s:%s:%s\n", str,
               SSL_alert_type_string_long(ret),
               SSL_alert_desc_string_long(ret));
    } else if (where & SSL_CB_EXIT) {
        if (ret == 0)
            printf("%s:failed in %s\n",
                   str, SSL_state_string_long(s));
        else if (ret < 0) {
            printf("%s:error in %s\n",
                   str, SSL_state_string_long(s));
        }
    }
}

void Server::stop() {
    acceptor_.close();
    acceptor_thread_.join();
    work_guard_->reset();
    if (io_context_thread_.joinable()) {
        io_context_thread_.join();
    }
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

        ClientConnection::Pointer new_session = ClientConnection::create(*io_context_, ssl_context_, *this);

        auto& socket = new_session->socket();
        acceptor_.accept(socket.lowest_layer(), ec);

        if(ec)
            std::cerr << "Accept Failed: " << ec.message() << std::endl;

        if (!ec) {
            new_session->setRemoteEndpoint(socket.lowest_layer().remote_endpoint());

            socket.async_handshake(boost::asio::ssl::stream_base::server,
                [this, new_session](const boost::system::error_code& ec) {
                    if (!ec) {
                        std::cout << "Connection Accepted: " << new_session->socket().lowest_layer().remote_endpoint() << std::endl;
                        on_session_started(new_session);
                        std::thread(&ClientConnection::start, new_session).detach();
                    } else {
                        std::cerr << "Handshake Failed: " << ec.message() << std::endl;
                    }
                }
            );
        }
    }
}
