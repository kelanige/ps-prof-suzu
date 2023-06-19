#include "session.hpp"

#include <string>

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/log/trivial.hpp>

namespace Pokemon {
  Session::Session(boost::asio::io_context& ioc, boost::asio::ssl::context& ctx) :
    ioc_(ioc)
  {}

  void Session::Run(const std::string& host, const std::string& port, const std::string& endpoint) {
    BOOST_LOG_TRIVIAL(trace) << "Run(" << host << "," << port << "," << endpoint << ")";
  }
}

