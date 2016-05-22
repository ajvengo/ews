/*
  Embedded web server based on Boost.Asio example "server3"
  originally created by Christopher M. Kohlhoff <chris@kohlhoff.com>

  Adapted by Vladimir Rapatskiy <rapatsky@gmail.com>
*/

#pragma once
#ifndef EWS_SERVER_HPP
#define EWS_SERVER_HPP

#include "connection.hpp"
#include "request_handler.hpp"

#include <boost/asio/io_service.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/noncopyable.hpp>

namespace ews {

namespace asio = boost::asio;
namespace ip  = boost::asio::ip;
using boost::system::error_code;

/// The top-level class of the HTTP server.
class server : private boost::noncopyable {
public:
  /// Construct the server to listen on the specified TCP address and port
  explicit server(unsigned short port, std::size_t thread_pool_size = 2);

  /// Run the server's io_service loop.
  void run();

private:
  /// Initiate an asynchronous accept operation.
  void start_accept();

  /// Handle completion of an asynchronous accept operation.
  void handle_accept(const error_code& e);

  /// Handle a request to stop the server.
  void handle_stop();

  std::size_t       thread_pool_size_;  ///< The number of threads that will call io_service::run().
  asio::io_service  io_service_;        ///< The io_service used to perform asynchronous operations.
  asio::signal_set  signals_;           ///< The signal_set is used to register for process termination notifications.
  ip::tcp::acceptor acceptor_;          ///< Acceptor used to listen for incoming connections.
  connection_ptr    new_connection_;    ///< The next connection to be accepted.
  request_handler   request_handler_;   ///< The handler for all incoming requests.
};

} // namespace ews

#endif // EWS_SERVER_HPP
