#include "session.h"

#include <chrono>
#include <string>

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/log/trivial.hpp>

namespace Pokemon {

  namespace {
    using ::boost::asio::bind_executor;
    using ::boost::beast::bind_front_handler;
    using ::boost::beast::get_lowest_layer;
    using ::boost::beast::make_printable;
  }

  Session::Session(boost::asio::io_context& ioc) :
    ioc_(ioc),
    resolver_(boost::asio::make_strand(ioc)),
    ws_(boost::asio::make_strand(ioc)),
    strand_(ioc.get_executor())
  {}

  void Session::Run(const std::string& host, const std::string& port, const std::string& endpoint) {
    BOOST_LOG_TRIVIAL(trace) << "Session::Run(" << host << "," << port << "," << endpoint << ")";
    host_ = host;
    endpoint_ = endpoint;

    resolver_.async_resolve(host.c_str(), port.c_str(), bind_front_handler(&Session::OnResolve, shared_from_this()));
  }

  // TODO: Print results in trace log.
  void Session::OnResolve(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type results) {
    if (ec) {
      BOOST_LOG_TRIVIAL(error) << ec.message();
      return;
    }
    BOOST_LOG_TRIVIAL(trace) << "Session::OnResolve(" << ec.to_string() << "," << "TODO: results" << ")";
    get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));
    get_lowest_layer(ws_).async_connect(results, bind_executor(strand_, bind_front_handler(&Session::OnConnect, shared_from_this())));
  }

  // TODO: Print ep in trace log.
  void Session::OnConnect(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type::endpoint_type ep) {
    if (ec) {
      BOOST_LOG_TRIVIAL(error) << ec.message();
      return;
    }
    BOOST_LOG_TRIVIAL(trace) << "Session::OnConnect(" << ec.to_string() << "," << "TODO: endpoint_type" << ")";
    get_lowest_layer(ws_).expires_never();

    ws_.set_option(boost::beast::websocket::stream_base::timeout::suggested(boost::beast::role_type::client));
    ws_.set_option(boost::beast::websocket::stream_base::decorator([](boost::beast::websocket::request_type& req) {
          req.set(boost::beast::http::field::user_agent, std::string(BOOST_BEAST_VERSION_STRING) + " websocket-client-async");
        }));
    host_ += ":" + std::to_string(ep.port());
    ws_.async_handshake(host_, endpoint_, bind_front_handler(&Session::OnHandshake, shared_from_this()));
  }

  void Session::OnHandshake(boost::beast::error_code ec) {
    if (ec) {
      BOOST_LOG_TRIVIAL(error) << ec.message();
      return;
    }
    BOOST_LOG_TRIVIAL(trace) << "Session::OnHandshake(" << ec.to_string() << ")";
    ws_.async_read(buffer_, bind_executor(strand_, bind_front_handler(&Session::OnRead, shared_from_this())));
  }

  void Session::OnWrite(boost::beast::error_code ec, std::size_t bytes) {
    if (ec) {
      BOOST_LOG_TRIVIAL(error) << ec.message();
      return;
    }
    BOOST_LOG_TRIVIAL(trace) << "Session::OnWrite(" << ec.to_string() << "," << bytes << ")";
    BOOST_LOG_TRIVIAL(debug) << "Buffer: " << make_printable(buffer_.data());
    ws_.async_read(buffer_, bind_executor(strand_, bind_front_handler(&Session::OnRead, shared_from_this())));
  }

  void Session::OnRead(boost::beast::error_code ec, std::size_t bytes) {
    if (ec) {
      BOOST_LOG_TRIVIAL(error) << ec.message();
      return;
    }
    BOOST_LOG_TRIVIAL(trace) << "Session::OnRead(" << ec.to_string() << "," << bytes << ")";
    BOOST_LOG_TRIVIAL(debug) << "Buffer: " << make_printable(buffer_.data());
    parser_.Parse(buffer_.data());
    buffer_.consume(bytes);
    ws_.async_close(boost::beast::websocket::close_code::normal, bind_front_handler(&Session::OnClose, shared_from_this()));
  }

  void Session::OnClose(boost::beast::error_code ec) {
    if (ec) {
      BOOST_LOG_TRIVIAL(error) << ec.message();
      return;
    }
    BOOST_LOG_TRIVIAL(trace) << "Session::OnClose(" << ec.to_string() << ")";
    BOOST_LOG_TRIVIAL(debug) << "Buffer: " << make_printable(buffer_.data());
  }

}

