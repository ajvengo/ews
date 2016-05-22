/*
  Embedded web server based on Boost.Asio example "server3"
  originally created by Christopher M. Kohlhoff <chris@kohlhoff.com>

  Adapted by Vladimir Rapatskiy <rapatsky@gmail.com>
*/

#include "connection.hpp"
#include "request_handler.hpp"
#include <boost/bind.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/write.hpp>

namespace ews {

namespace asio = boost::asio;
namespace ip  = boost::asio::ip;
namespace ph = boost::asio::placeholders;
using boost::system::error_code;

connection::connection(asio::io_service& io_service, request_handler& handler)
  : strand_(io_service),
    socket_(io_service),
    timer_(io_service),
    request_handler_(handler) {
  request_.method.reserve(8);
  request_.uri.reserve(256);
  request_.headers.reserve(16);
  request_.body.reserve(256);
}

ip::tcp::socket& connection::socket() {
  return socket_;
}

void connection::start() {
  socket_.async_read_some(
    asio::buffer(buffer_),
    strand_.wrap(boost::bind(&connection::handle_read, shared_from_this(), ph::error, ph::bytes_transferred))
  );
}

void connection::close() {
  error_code ec;
  socket_.shutdown(asio::socket_base::shutdown_both, ec);
  socket_.close(ec);
}

void connection::handle_timer() {
  asio::async_write(
    socket_, reply_.to_buffers(),
    strand_.wrap(boost::bind(&connection::handle_write, shared_from_this(), ph::error))
  );
  --data_.attempts;
  if (!data_.attempts) {
    close();
    return;
  }
  timer_.expires_at(timer_.expires_at() + data_.interval);
  timer_.async_wait(boost::bind(&connection::handle_timer, shared_from_this()));
}

void connection::handle_read(const error_code& e, std::size_t bytes_transferred) {
  if (!e) {
    boost::tribool result;
    boost::tie(result, boost::tuples::ignore) =
      request_parser_.parse(request_, buffer_.data(), buffer_.data() + bytes_transferred);

    if (result) {
      request_handler_.handle_request(request_, reply_, data_);
      if (data_.status == json_data::ok && data_.attempts) {
        timer_.expires_from_now(boost::posix_time::seconds(0));
        handle_timer();
      }
    } else if (!result) {
      reply_ = reply::stock_reply(reply::bad_request, "HTTP request parse error");
      asio::async_write(
        socket_, reply_.to_buffers(),
        strand_.wrap(boost::bind(&connection::handle_write, shared_from_this(), ph::error))
      );
    } else {
      socket_.async_read_some(
        asio::buffer(buffer_),
        strand_.wrap(boost::bind(&connection::handle_read, shared_from_this(), ph::error, ph::bytes_transferred))
      );
    }
  }

  // If an error occurs then no new asynchronous operations are started. This
  // means that all shared_ptr references to the connection object will
  // disappear and the object will be destroyed automatically after this
  // handler returns. The connection class's destructor closes the socket.
}

void connection::handle_write(const error_code& e) {
}

} // namespace ews

