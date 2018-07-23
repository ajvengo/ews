/*
  Embedded web server based on Boost.Asio example "server3"
  originally created by Christopher M. Kohlhoff <chris@kohlhoff.com>

  Adapted by Vladimir Rapatskiy <rapatsky@gmail.com>
*/

#pragma once
#ifndef EWS_CONNECTION_HPP
#define EWS_CONNECTION_HPP

#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/deadline_timer.hpp>

#include "reply.hpp"
#include "request.hpp"
#include "request_parser.hpp"
#include "json_data.hpp"

namespace ews {

namespace asio = boost::asio;
namespace ip  = boost::asio::ip;
using boost::system::error_code;

struct request_handler;

/// Represents a single connection from a client.
class connection
  : public boost::enable_shared_from_this<connection>,
    private boost::noncopyable {

public:
  /// Construct a connection with the given io_service.
  explicit connection(asio::io_service& io_service, request_handler& handler);

  /// Get the socket associated with the connection.
  ip::tcp::socket& socket();

  /// Start the first asynchronous operation for the connection.
  void start();

private:
  /// Close socket
  void close();

  /// Handle completion of a read operation.
  void handle_read(const error_code& e, std::size_t bytes_transferred);

  /// Handle completion of a write operation.
  void handle_write(const error_code& e);

  /// Handle timer for next send message attempt
  void handle_timer();

  asio::io_service::strand  strand_;            ///< Strand to ensure the connection's handlers are not called concurrently.
  ip::tcp::socket           socket_;            ///< Socket for the connection.
  asio::deadline_timer      timer_;             ///< Timer for repeating reply
  request_handler&          request_handler_;   ///< The handler used to process the incoming request.
  boost::array<char, 8192>  buffer_;            ///< Buffer for incoming data.
  request                   request_;           ///< The incoming request.
  request_parser            request_parser_;    ///< The parser for the incoming request.
  reply                     reply_;             ///< The reply to be sent back to the client.
  json_data                 data_;              ///< JSON request data
};

using connection_ptr = boost::shared_ptr<connection>;

} // namespace ews

#endif // EWS_CONNECTION_HPP
