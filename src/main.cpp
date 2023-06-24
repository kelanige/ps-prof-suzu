#include <iostream>
#include <memory>

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>

#include "session.hpp"

int main(int argc, char** argv) {
  boost::asio::io_context ioc;
  boost::asio::ssl::context ctx{boost::asio::ssl::context::tlsv12_client};
  ctx.set_verify_mode(boost::asio::ssl::context::verify_none);
  ctx.set_default_verify_paths();
  std::shared_ptr<Pokemon::Session> session = std::make_shared<Pokemon::Session>(ioc, ctx);
  session->Run("localhost", "8000", "/showdown/websocket");
  ioc.run();
  return EXIT_SUCCESS;
}
