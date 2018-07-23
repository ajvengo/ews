/*
  Embedded web server based on Boost.Asio example "server3"
  originally created by Christopher M. Kohlhoff <chris@kohlhoff.com>

  Adapted by Vladimir Rapatskiy <rapatsky@gmail.com>
*/

#include "server.hpp"

#include <iostream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
  try {
    unsigned short  port;
    std::size_t     num_threads;

    // Parse command line options
    po::options_description desc("Embedded Web Server, echo short messages using JSON\nAllowed options");
    desc.add_options()
        ("help,h", "print options summary")
        ("port,p", po::value<unsigned short>(&port)->default_value(8080), "port number")
        ("threads,t", po::value<std::size_t>(&num_threads)->default_value(2), "threads number")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
      std::cout << desc << '\n';
      return 0;
    }

    // Run the server until stopped.
    ews::server s(port, num_threads);
    s.run();
  } catch (std::exception& e) {
    std::cerr << "exception: " << e.what() << '\n';
  }

  return 0;
}
