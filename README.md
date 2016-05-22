# README #

Embedded Web Server using Boost.Asio

### What is this repository for? ###

* It is a sample of HTTP client and server made asynchronous using Boost.Asio
* Server is able to serve 10k connections per second on Linux
* Stress test client included
* To get list of executable options use "-h" or "--help"

### How do I get set up? ###

* Use CMake 3.0.0+, installer link: https://cmake.org/download
* For VS 2015: run configure.cmd from build/vs2015 directory
* For VS 2013: run configure.cmd from build/vs2013 directory
* For VS 2010: run configure.cmd from build/vs2010 directory
* For Linux: run build.sh from build/linux directory
** Under Linux apropriate file descriptors limits must be set using "ulimit -n fd_limit_number" before starting EWS server
* Binaries will be created in "bin" directory
