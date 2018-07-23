/*
  Embedded web server based on Boost.Asio example "server3"
  originally created by Christopher M. Kohlhoff <chris@kohlhoff.com>

  Adapted by Vladimir Rapatskiy <rapatsky@gmail.com>
*/

#include "request_parser.hpp"
#include "request.hpp"


namespace ews {

void request_parser::reset() {
  state_ = method_start;
  nesting_level_ = 0;
}

boost::tribool request_parser::consume(request& req, char input) {
  switch (state_) {
  case method_start:
    if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
      return false;
    } else {
      state_ = method;
      req.method.push_back(input);
      return boost::indeterminate;
    }
  case method:
    if (input == ' ') {
      if (req.method != "POST") return false; // only POST method is allowed
      state_ = uri;
      return boost::indeterminate;
    } else if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
      return false;
    } else {
      req.method.push_back(input);
      return boost::indeterminate;
    }
  case uri:
    if (input == ' ') {
      state_ = http_version_h;
      return boost::indeterminate;
    } else if (is_ctl(input) || input == '?') {
      return false;
    } else {
      req.uri.push_back(input);
      return boost::indeterminate;
    }
  case http_version_h:
    if (input == 'H') {
      state_ = http_version_t_1;
      return boost::indeterminate;
    } else {
      return false;
    }
  case http_version_t_1:
    if (input == 'T') {
      state_ = http_version_t_2;
      return boost::indeterminate;
    } else {
      return false;
    }
  case http_version_t_2:
    if (input == 'T') {
      state_ = http_version_p;
      return boost::indeterminate;
    } else {
      return false;
    }
  case http_version_p:
    if (input == 'P') {
      state_ = http_version_slash;
      return boost::indeterminate;
    } else {
      return false;
    }
  case http_version_slash:
    if (input == '/') {
      req.http_version_major = 0;
      req.http_version_minor = 0;
      state_ = http_version_major_start;
      return boost::indeterminate;
    } else {
      return false;
    }
  case http_version_major_start:
    if (is_digit(input)) {
      req.http_version_major = input - '0';
      state_ = http_version_major;
      return boost::indeterminate;
    } else {
      return false;
    }
  case http_version_major:
    if (input == '.') {
      state_ = http_version_minor_start;
      return boost::indeterminate;
    } else if (is_digit(input)) {
      req.http_version_major = req.http_version_major * 10 + input - '0';
      return boost::indeterminate;
    } else {
      return false;
    }
  case http_version_minor_start:
    if (is_digit(input)) {
      req.http_version_minor = input - '0';
      state_ = http_version_minor;
      return boost::indeterminate;
    } else {
      return false;
    }
  case http_version_minor:
    if (input == '\r') {
      state_ = expecting_newline_before_first_header;
      return boost::indeterminate;
    } else if (is_digit(input)) {
      req.http_version_minor = req.http_version_minor * 10 + input - '0';
      return boost::indeterminate;
    } else {
      return false;
    }
  case expecting_newline_before_first_header:
    if (input == '\n') {
      state_ = header_line_start;
      return boost::indeterminate;
    } else {
      return false;
    }
  case header_line_start:
    if (input == '\r') {
      state_ = expecting_body_start;
      return boost::indeterminate;
    } else if (!req.headers.empty() && (input == ' ' || input == '\t')) {
      state_ = header_lws;
      return boost::indeterminate;
    } else if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
      return false;
    } else {
      req.headers.emplace_back();
      header& h = req.headers.back();
      h.name.reserve(64);
      h.value.reserve(64);
      h.name.push_back(input);
      state_ = header_name;
      return boost::indeterminate;
    }
  case header_lws:
    if (input == '\r') {
      state_ = expecting_newline;
      return boost::indeterminate;
    } else if (input == ' ' || input == '\t') {
      return boost::indeterminate;
    } else if (is_ctl(input)) {
      return false;
    } else {
      state_ = header_value;
      req.headers.back().value.push_back(input);
      return boost::indeterminate;
    }
  case header_name:
    if (input == ':') {
      state_ = space_before_header_value;
      return boost::indeterminate;
    } else if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
      return false;
    } else {
      req.headers.back().name.push_back(input);
      return boost::indeterminate;
    }
  case space_before_header_value:
    if (input == ' ') {
      state_ = header_value;
      return boost::indeterminate;
    } else {
      return false;
    }
  case header_value:
    if (input == '\r') {
      state_ = expecting_newline;
      return boost::indeterminate;
    } else if (is_ctl(input)) {
      return false;
    } else {
      req.headers.back().value.push_back(input);
      return boost::indeterminate;
    }
  case expecting_newline:
    if (input == '\n') {
      state_ = header_line_start;
      return boost::indeterminate;
    } else {
      return false;
    }
  case expecting_body_start:
    if (input == '\n') {
      state_ = expecting_json_start;
      return boost::indeterminate;
    } else {
      return false;
    }
  case expecting_json_start:
    req.body.push_back(input);
    if (input == '{') {
      ++nesting_level_;
      state_ = expecting_json_end;
    }
    return boost::indeterminate;
  case expecting_json_end:
    req.body.push_back(input);
    if (input == '{') {
      ++nesting_level_;
    } else if (input == '}') {
      --nesting_level_;
      if (!nesting_level_) return true;
    }
    return boost::indeterminate;
  default:
    return false;
  }
}

bool request_parser::is_char(int c) {
  return c >= 0 && c <= 127;
}

bool request_parser::is_ctl(int c) {
  return (c >= 0 && c <= 31) || (c == 127);
}

bool request_parser::is_tspecial(int c) {
  switch (c) {
  case '(':
  case ')':
  case '<':
  case '>':
  case '@':
  case ',':
  case ';':
  case ':':
  case '\\':
  case '"':
  case '/':
  case '[':
  case ']':
  case '?':
  case '=':
  case '{':
  case '}':
  case ' ':
  case '\t':
    return true;
  default:
    return false;
  }
}

bool request_parser::is_digit(int c) {
  return c >= '0' && c <= '9';
}

} // namespace ews

