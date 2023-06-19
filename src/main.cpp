#include <iostream>
#include <memory>

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>

#include "session.hpp"

int main(int argc, char** argv) {
  boost::asio::io_context ioc;
  boost::asio::ssl::context ctx{boost::asio::ssl::context::tlsv12_client};
  std::shared_ptr<Pokemon::Session> session = std::make_shared<Pokemon::Session>(ioc, ctx);
  session->Run("wss://sim3.psim.us", "443", "showdown/websocket");
  return EXIT_SUCCESS;
}
