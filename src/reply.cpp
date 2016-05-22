/*
  Embedded web server based on Boost.Asio example "server3"
  originally created by Christopher M. Kohlhoff <chris@kohlhoff.com>

  Adapted by Vladimir Rapatskiy <rapatsky@gmail.com>
*/

#include "reply.hpp"
#include "json_data.hpp"
#include <string>
#include <boost/lexical_cast.hpp>

namespace ews {

namespace asio = boost::asio;

namespace status_strings {

const std::string ok =
  "HTTP/1.0 200 OK\r\n";
const std::string created =
  "HTTP/1.0 201 Created\r\n";
const std::string accepted =
  "HTTP/1.0 202 Accepted\r\n";
const std::string no_content =
  "HTTP/1.0 204 No Content\r\n";
const std::string multiple_choices =
  "HTTP/1.0 300 Multiple Choices\r\n";
const std::string moved_permanently =
  "HTTP/1.0 301 Moved Permanently\r\n";
const std::string moved_temporarily =
  "HTTP/1.0 302 Moved Temporarily\r\n";
const std::string not_modified =
  "HTTP/1.0 304 Not Modified\r\n";
const std::string bad_request =
  "HTTP/1.0 400 Bad Request\r\n";
const std::string unauthorized =
  "HTTP/1.0 401 Unauthorized\r\n";
const std::string forbidden =
  "HTTP/1.0 403 Forbidden\r\n";
const std::string not_found =
  "HTTP/1.0 404 Not Found\r\n";
const std::string internal_server_error =
  "HTTP/1.0 500 Internal Server Error\r\n";
const std::string not_implemented =
  "HTTP/1.0 501 Not Implemented\r\n";
const std::string bad_gateway =
  "HTTP/1.0 502 Bad Gateway\r\n";
const std::string service_unavailable =
  "HTTP/1.0 503 Service Unavailable\r\n";

asio::const_buffer to_buffer(reply::status_type status) {
  switch (status) {
  case reply::ok:
    return asio::buffer(ok);
  case reply::bad_request:
    return asio::buffer(bad_request);
  case reply::internal_server_error:
    return asio::buffer(internal_server_error);
  case reply::not_implemented:
    return asio::buffer(not_implemented);
  case reply::service_unavailable:
    return asio::buffer(service_unavailable);
  default:
    return asio::buffer(internal_server_error);
  }
}

} // namespace status_strings

namespace misc_strings {

const char name_value_separator[] = { ':', ' ' };
const char crlf[] = { '\r', '\n' };

} // namespace misc_strings

std::vector<asio::const_buffer> reply::to_buffers() {
  std::vector<asio::const_buffer> buffers;
  buffers.push_back(status_strings::to_buffer(status));
  for (std::size_t i = 0; i < headers.size(); ++i) {
    const header& h = headers[i];
    buffers.push_back(asio::buffer(h.name));
    buffers.push_back(asio::buffer(misc_strings::name_value_separator));
    buffers.push_back(asio::buffer(h.value));
    buffers.push_back(asio::buffer(misc_strings::crlf));
  }
  buffers.push_back(asio::buffer(misc_strings::crlf));
  buffers.push_back(asio::buffer(body));
  return buffers;
}

reply reply::stock_reply(reply::status_type status, const std::string& error_message) {
  reply rep;
  rep.status = status;
  rep.body = json_data::make_body("error", error_message);
  rep.headers.resize(2);
  rep.headers[0].name = "Content-Length";
  rep.headers[0].value = boost::lexical_cast<std::string>(rep.body.size());
  rep.headers[1].name = "Content-Type";
  rep.headers[1].value = "application/json";
  return rep;
}

} // namespace ews
