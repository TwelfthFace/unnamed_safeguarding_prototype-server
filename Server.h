#pragma once

#include <boost/asio.hpp>
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>
#include <set>
#include <mutex>

#include "ClientConnection.h"

class ClientConnection;

class Server{
public:
  Server(boost::asio::io_context &context, unsigned short port);
  //typedef boost::shared_ptr<ClientConnection> Pointer;
  std::set<ClientConnection::Pointer> getClientConnections();
  void RemoveClosedConnections();
  void on_session_started(ClientConnection::Pointer connection);
  void on_session_stopped(ClientConnection::Pointer connection, const boost::asio::ip::tcp::endpoint& endpoint);
  void start();
  void stop();
private:
  void acceptConnection();
  boost::asio::ip::tcp::acceptor acceptor_;
  std::set<ClientConnection::Pointer> active_connections_;
  std::thread acceptor_thread_;
  std::mutex connections_mutex_;
};
