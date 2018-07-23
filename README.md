# README #

Embedded Web Server using Boost.Asio

### What is this repository for? ###

* It is a sample of HTTP client and server made asynchronous using Boost.Asio.
* Server is able to serve 10k connections per second.

### How do I get set up? ###

* Use CMake 3.8+
* Unpack sources
* For VS 2017: run configure.cmd from vs2017 directory
* For Linux: run build.sh from linux directory
* Binaries will be created in "bin" directory
* Under Linux apropriate file descriptors limits must be set using "ulimit -n fd_limit_number" before starting EWS server
