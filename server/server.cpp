/*
  Embedded web server based on Boost.Asio example "server3"
  originally created by Christopher M. Kohlhoff <chris@kohlhoff.com>

  Adapted by Vladimir Rapatskiy <rapatsky@gmail.com>
*/

#include "server.hpp"
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/asio/placeholders.hpp>
#include <vector>

namespace ews {

namespace asio = boost::asio;
namespace ip  = boost::asio::ip;
namespace ph = boost::asio::placeholders;
using boost::system::error_code;
using boost::shared_ptr;
using boost::make_shared;

server::server(unsigned short port, std::size_t thread_pool_size)
  : thread_pool_size_(thread_pool_size),
    signals_(io_service_),
    acceptor_(io_service_),
    new_connection_(),
    request_handler_() {

  // Register to handle the signals that indicate when the server should exit.
  // It is safe to register for the same signal multiple times in a program,
  // provided all registration for the specified signal is made through Asio.
  signals_.add(SIGINT);
  signals_.add(SIGTERM);
#ifdef SIGQUIT
  signals_.add(SIGQUIT);
#endif // defined(SIGQUIT)
  signals_.async_wait(boost::bind(&server::handle_stop, this));

  // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
  ip::tcp::endpoint endpoint(ip::address_v4::loopback(), port);
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();
  start_accept();
}

void server::run() {
  // Create a pool of threads to run all of the io_services.
  using thread_ptr = shared_ptr<boost::thread>;
  std::vector<thread_ptr> threads;
  threads.reserve(thread_pool_size_);
  for (std::size_t i = 0; i < thread_pool_size_; ++i) {
    threads.push_back(make_shared<boost::thread>(boost::bind(&asio::io_service::run, &io_service_)));
  }

  // Wait for all threads in the pool to exit.
  for (std::size_t i = 0; i < thread_pool_size_; ++i)
    threads[i]->join();
}

void server::start_accept() {
  new_connection_.reset(new connection(io_service_, request_handler_));
  acceptor_.async_accept(
    new_connection_->socket(),
    boost::bind(&server::handle_accept, this, ph::error)
  );
}

void server::handle_accept(const error_code& e) {
  if (!e) {
    new_connection_->start();
  }
  start_accept();
}

void server::handle_stop() {
  io_service_.stop();
}

} // namespace ews
