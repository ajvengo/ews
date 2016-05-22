/*
  Embedded web server based on Boost.Asio example "server3"
  originally created by Christopher M. Kohlhoff <chris@kohlhoff.com>

  Adapted by Vladimir Rapatskiy <rapatsky@gmail.com>
*/

#pragma once
#ifndef EWS_REPLY_HPP
#define EWS_REPLY_HPP

#include "header.hpp"
#include <boost/asio/buffer.hpp>
#include <string>
#include <vector>

namespace ews {

namespace asio = boost::asio;

/// A reply to be sent to a client.
struct reply {
  /// The status of the reply.
  enum status_type {
    ok = 200,
    bad_request = 400,
    internal_server_error = 500,
    not_implemented = 501,
    service_unavailable = 503
  } status;

  /// Get a stock reply.
  static reply stock_reply(status_type status, const std::string& error_message);

  /// The headers to be included in the reply.
  std::vector<header> headers;

  /// The content to be sent in the reply.
  std::string body;

  /// Convert the reply into a vector of buffers. The buffers do not own the
  /// underlying memory blocks, therefore the reply object must remain valid and
  /// not be changed until the write operation has completed.
  std::vector<asio::const_buffer> to_buffers();
};

} // namespace ews

#endif // EWS_REPLY_HPP
