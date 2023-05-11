#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <openssl/ssl.h>
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>
#include <set>
#include <mutex>

#include "ClientConnection.h"

class ClientConnection;

class Server {
public:
  Server(unsigned short port);
  std::set<ClientConnection::Pointer> getClientConnections();
  void RemoveClosedConnections();
  void on_session_started(ClientConnection::Pointer connection);
  void on_session_stopped(ClientConnection::Pointer connection, const boost::asio::ip::tcp::endpoint& endpoint);
  void start();
  void stop();
private:
  void acceptConnection();
  static void debug_callback(const SSL *s, int where, int ret);

  std::unique_ptr<boost::asio::io_context> io_context_;
  boost::asio::ip::tcp::acceptor acceptor_;
  boost::asio::ssl::context ssl_context_;
  std::set<ClientConnection::Pointer> active_connections_;
  std::thread acceptor_thread_;
  std::mutex connections_mutex_;
  std::unique_ptr<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>> work_guard_;
  std::thread io_context_thread_;
};
