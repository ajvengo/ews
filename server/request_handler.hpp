/*
  Embedded web server based on Boost.Asio example "server3"
  originally created by Christopher M. Kohlhoff <chris@kohlhoff.com>

  Adapted by Vladimir Rapatskiy <rapatsky@gmail.com>
*/

#pragma once
#ifndef EWS_REQUEST_HANDLER_HPP
#define EWS_REQUEST_HANDLER_HPP

#include <boost/noncopyable.hpp>

namespace ews {

struct reply;
struct request;
struct json_data;

/// The common handler for all incoming requests.
struct request_handler : private boost::noncopyable {
  /// Handle a request, validate it and produce a reply.
  static void handle_request(const request& req, reply& rep, json_data& data);
};

} // namespace ews

#endif // EWS_REQUEST_HANDLER_HPP
