/*
  Embedded web server stress test based on Boost.Asio example "client/async_client"
  originally created by Christopher M. Kohlhoff <chris@kohlhoff.com>

  Adapted by Vladimir Rapatskiy <rapatsky@gmail.com>
*/

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/make_shared.hpp>
#include <boost/program_options.hpp>
#include <boost/atomic.hpp>
#include <cstdint>

namespace asio = boost::asio;
namespace ip  = boost::asio::ip;
namespace ph = boost::asio::placeholders;
namespace rnd = boost::random;
namespace po = boost::program_options;
using boost::system::error_code;
using boost::asio::ip::tcp;
using boost::posix_time::seconds;
using boost::posix_time::microseconds;
using boost::bind;
using boost::shared_ptr;
using boost::make_shared;
using boost::ref;
using boost::atomic_int64_t;
using std::string;
using std::cout;
using std::endl;

struct errors_counters_t {
  atomic_int64_t socket_open_errors;
  atomic_int64_t bind_errors;
  atomic_int64_t connect_errors;
  atomic_int64_t send_errors;
  atomic_int64_t receive_errors;
  atomic_int64_t http_status_errors;
  atomic_int64_t reply_success;

  errors_counters_t() :
    socket_open_errors(0),
    bind_errors(0),
    connect_errors(0),
    send_errors(0),
    receive_errors(0),
    http_status_errors(0),
    reply_success(0) {}

  ~errors_counters_t() {
    cout
      << "Errors summary\nSocket open errors: " << socket_open_errors << "\nBind errors: " << bind_errors << "\nConnect errors: " << connect_errors
      << "\nSend errors: " << send_errors << "\nReceive errors: " << receive_errors << "\nHTTP status errors: " << http_status_errors
      << "\nSuccessful reply: " << reply_success << endl;
  }
};

class connection :
  public boost::enable_shared_from_this<connection>,
  private boost::noncopyable {

public:
  connection(asio::io_service& io_service, const ip::tcp::endpoint& local, const ip::tcp::endpoint& remote, errors_counters_t& error_counters) :
    socket_(io_service), remote_(remote), error_counters_(error_counters) {
    error_code ec;
    if (socket_.open(ip::tcp::v4(), ec)) {
      ++error_counters_.socket_open_errors;
      return;
    }
    socket_.set_option(asio::socket_base::reuse_address(true));
    if (socket_.bind(local, ec)) {
      ++error_counters_.bind_errors;
      socket_.close();
      return;
    }

    // make request
    std::ostream request_stream(&request_);
    request_stream << "POST localhost:8080 HTTP/1.0\r\n";
    request_stream << "Host: localhost\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Connection: close\r\n";
    request_stream << "Content-Type: application/json\r\n";
    const string json = "{\n\"data\":{\n\"message\": \"Hi\",\n\"attempts\": 1,\n\"interval\": 1}\n}";
    request_stream << "Content-Length: " << json.size() << "\r\n\r\n" << json;
  }

  void connect() {
    if (socket_.is_open())
      socket_.async_connect(remote_, bind(&connection::handle_connect, shared_from_this(), ph::error));
  }

  ~connection() {
    if (socket_.is_open()) socket_.close();
  }

private:
  void handle_connect(const error_code& e) {
    if (!e) {
      // The connection was successful. Send the request.
      asio::async_write(
        socket_, request_,
        bind(&connection::handle_write_request, shared_from_this(), ph::error)
      );
    } else {
      ++error_counters_.connect_errors;
    }
  }

  void handle_write_request(const error_code& e) {
    if (!e) {
      asio::async_read(
        socket_, response_, asio::transfer_at_least(12),
        boost::bind(&connection::handle_read_status_line, shared_from_this(), ph::error)
      );
    } else {
      ++error_counters_.send_errors;
    }
  }

  void handle_read_status_line(const error_code& e) {
    if (!e) {
      // Check that response is OK.
      std::istream response_stream(&response_);
      string http_version;
      unsigned int status_code;
      response_stream >> http_version >> status_code;
      if (!response_stream || http_version.substr(0, 5) != "HTTP/" || status_code != 200) {
        ++error_counters_.http_status_errors;
      } else {
        ++error_counters_.reply_success;
        error_code ec;
        socket_.shutdown(asio::socket_base::shutdown_both, ec);
        socket_.close(ec);
      }
    } else {
      ++error_counters_.receive_errors;
    }
  }

  ip::tcp::socket           socket_;            ///< Socket for the connection.
  const ip::tcp::endpoint&  remote_;
  errors_counters_t&        error_counters_;
  asio::streambuf           request_;
  asio::streambuf           response_;
};
using connection_ptr = shared_ptr<connection>;

class stress_test_client {
public:
  stress_test_client(asio::io_service& io_service, unsigned short port = 8080, unsigned connection_rate = 10000u, unsigned duration = 2u)
    : io_service_(io_service),
      connect_timer_(io_service),
      stop_timer_(io_service),
      remote_(ip::address_v4::loopback(), port),
      rnd_port_(9000, 32700),
      connect_time_delta_(1000000u / connection_rate) {

    stop_timer_.expires_from_now(seconds(duration));
    stop_timer_.async_wait(bind(&stress_test_client::handle_stop, this));
    connect_timer_.async_wait(bind(&stress_test_client::handle_connect_timer, this));
    connect_timer_.expires_from_now(seconds(0));
    io_service_.run();
  }

private:
  void handle_connect_timer() {
    const auto port = static_cast<uint16_t>(rnd_port_(rnd_source_));
    const ip::tcp::endpoint local(ip::address_v4::loopback(), port);
    new_connection_ = make_shared<connection>(ref(io_service_), local, remote_, ref(error_counters_));
    new_connection_->connect();
    connect_timer_.expires_at(connect_timer_.expires_at() + connect_time_delta_);
    connect_timer_.async_wait(bind(&stress_test_client::handle_connect_timer, this));
  }

  void handle_stop() {
    io_service_.stop();
  }

  asio::io_service&                 io_service_;
  asio::deadline_timer              connect_timer_;
  asio::deadline_timer              stop_timer_;
  connection_ptr                    new_connection_;
  ip::tcp::endpoint                 remote_;
  rnd::uniform_int_distribution<>   rnd_port_;
  rnd::mt19937                      rnd_source_;
  microseconds                      connect_time_delta_;
  errors_counters_t                 error_counters_;
};

int main(int argc, char* argv[]) {
  try {
    unsigned short  port;
    unsigned        rate;
    unsigned        duration;

    // Parse command line options
    po::options_description desc("Stress testing client for Embedded Web Server\nAllowed options");
    desc.add_options()
        ("help,h", "print options summary")
        ("port,p", po::value<unsigned short>(&port)->default_value(8080), "port number")
        ("rate,r", po::value<unsigned>(&rate)->default_value(10000), "connections per second")
        ("time,t", po::value<unsigned>(&duration)->default_value(5), "test duration in seconds")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
      std::cout << desc << '\n';
      return 0;
    }

    asio::io_service io_service;
    stress_test_client c(io_service, port, rate, duration);
  } catch (const std::exception& e) {
    cout << e.what() << endl;
  }
  return 0;
}
