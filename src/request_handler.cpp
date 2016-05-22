/*
  Embedded web server based on Boost.Asio example "server3"
  originally created by Christopher M. Kohlhoff <chris@kohlhoff.com>

  Adapted by Vladimir Rapatskiy <rapatsky@gmail.com>
*/

#include "request_handler.hpp"

#include "reply.hpp"
#include "request.hpp"
#include "json_data.hpp"
#include <boost/lexical_cast.hpp>

namespace ews {

void request_handler::handle_request(const request& req, reply& rep, json_data& data) {
  data.status = data.parse(req.body);
  if (data.status != json_data::ok) {
    rep = reply::stock_reply(reply::bad_request, json_data::status_message(data.status));
    return;
  }

  // Fill out the reply to be sent to the client.
  rep.status = reply::ok;
  rep.body = json_data::make_body("data", data.message);
  rep.headers.resize(2);
  rep.headers[0].name = "Content-Length";
  rep.headers[0].value = boost::lexical_cast<std::string>(rep.body.size());
  rep.headers[1].name = "Content-Type";
  rep.headers[1].value = "application/json";
}

} // namespace ews
