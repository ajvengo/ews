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

  std::string                   message;    ///< short message, which will be replied to client
  unsigned                      attempts;   ///< number of attempts
  boost::posix_time::millisec   interval;   ///< interval between attempts
  status_type                   status;     ///< JSON parsing result status

  json_data() : attempts(0), interval(1000), status(missing_data) {}

  /// Parse JSON payload
  status_type parse(const std::string& str);

  /// Get status description
  static const std::string& status_message(status_type status);

  /// Make reply body in JSON
  static std::string make_body(const std::string& tag, const std::string& value);
};

} // namespace ews

#endif // EWS_JSON_DATA_HPP
