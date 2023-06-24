#include <iostream>
#include <memory>

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>

#include "session.hpp"

int main(int argc, char** argv) {
  boost::asio::io_context ioc;
  std::shared_ptr<Pokemon::Session> session = std::make_shared<Pokemon::Session>(ioc);
  session->Run("localhost", "8000", "/showdown/websocket");
  ioc.run();
  return EXIT_SUCCESS;
}
