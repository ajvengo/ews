/*
  Embedded web server based on Boost.Asio example "server3"
  originally created by Christopher M. Kohlhoff <chris@kohlhoff.com>

  Adapted by Vladimir Rapatskiy <rapatsky@gmail.com>
*/

#pragma once
#ifndef EWS_JSON_DATA_HPP
#define EWS_JSON_DATA_HPP

#include <string>
#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace ews {

/// Request payload data parser
struct json_data {
  /// JSON parsing status
  enum status_type {
    ok,
    json_parse_error,
    missing_data,
    missing_message,
    missing_attempts,
    missing_interval,
    message_not_string,
    attempts_not_integer,
    interval_not_number
  };

  std::string                   message;                ///< short message, which will be replied to client
  boost::posix_time::millisec   interval{1000};         ///< interval between attempts
  unsigned                      attempts{0};            ///< number of attempts
  status_type                   status{missing_data};   ///< JSON parsing result status

  /// Parse JSON payload
  status_type parse(const std::string& str);

  /// Get status description
  static const std::string& status_message(status_type status);

  /// Make reply body in JSON
  static const std::string make_body(const std::string& tag, const std::string& value);
};

} // namespace ews

#endif // EWS_JSON_DATA_HPP
