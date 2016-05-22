/*
  Embedded web server based on Boost.Asio example "server3"
  originally created by Christopher M. Kohlhoff <chris@kohlhoff.com>

  Adapted by Vladimir Rapatskiy <rapatsky@gmail.com>
*/

#pragma once
#ifndef EWS_HEADER_HPP
#define EWS_HEADER_HPP

#include <string>

namespace ews {

struct header {
  std::string name;
  std::string value;
};

} // namespace ews

#endif // EWS_HEADER_HPP
