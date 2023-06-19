#include <iostream>

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>

#include "session.hpp"

int main(int argc, char** argv) {
  boost::asio::io_context ioc;
  boost::asio::ssl::context ctx{boost::asio::ssl::context::tlsv12_client};
  Pokemon::Session session(ioc, ctx);
  session.Run("host", "port", "endpoint");
  return EXIT_SUCCESS;
}
