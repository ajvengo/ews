/*
  Embedded web server based on Boost.Asio example "server3"
  originally created by Christopher M. Kohlhoff <chris@kohlhoff.com>

  Adapted by Vladimir Rapatskiy <rapatsky@gmail.com>
*/

#pragma once
#ifndef EWS_REQUEST_HPP
#define EWS_REQUEST_HPP

#include <string>
#include <vector>
#include "header.hpp"

namespace ews {

/// A request received from a client.
struct request {
  std::string           method;
  std::string           uri;
  int                   http_version_major{0};
  int                   http_version_minor{0};
  std::vector<header>   headers;
  std::string           body;
};

} // namespace ews

#endif // EWS_REQUEST_HPP
