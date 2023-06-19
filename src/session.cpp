#include "session.hpp"

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
  }

  Session::Session(boost::asio::io_context& ioc, boost::asio::ssl::context& ctx) :
    ioc_(ioc),
    resolver_(boost::asio::make_strand(ioc)),
    ws_(boost::asio::make_strand(ioc), ctx),
    strand_(ioc.get_executor())
  {}

  void Session::Run(const std::string& host, const std::string& port, const std::string& endpoint) {
    BOOST_LOG_TRIVIAL(trace) << "Run(" << host << "," << port << "," << endpoint << ")";
    host_ = host;
    endpoint_ = endpoint;

    resolver_.async_resolve(host.c_str(), port.c_str(), bind_front_handler(&Session::OnResolve, shared_from_this()));
  }

  // TODO: Print results in trace log.
  void Session::OnResolve(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type results) {
    BOOST_LOG_TRIVIAL(trace) << "OnResolve(" << ec.to_string() << "," << "TODO: results" << ")";
    get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));
    get_lowest_layer(ws_).async_connect(results, bind_executor(strand_, bind_front_handler(&Session::OnConnect, shared_from_this())));
  }

  // TODO: Print ep in trace log.
  void Session::OnConnect(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type::endpoint_type ep) {
    BOOST_LOG_TRIVIAL(trace) << "OnConnect(" << ec.to_string() << "," << "TODO: endpoint_type" << ")";
    get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));
    
    if (!SSL_set_tlsext_host_name(ws_.next_layer().native_handle(), host_.c_str())) {
      ec = boost::beast::error_code(static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category());
      BOOST_LOG_TRIVIAL(error) << "OnConnect: " << ec.message();
    }

    host_ += ":" + std::to_string(ep.port());
    ws_.next_layer().async_handshake(boost::asio::ssl::stream_base::client, bind_executor(strand_, bind_front_handler(&Session::OnSSLHandshake, shared_from_this())));
  }

  void Session::OnSSLHandshake(boost::beast::error_code ec) {
    BOOST_LOG_TRIVIAL(trace) << "OnSSLHandshake(" << ec.to_string() << ")";
    get_lowest_layer(ws_).expires_never();
    ws_.set_option(boost::beast::websocket::stream_base::timeout::suggested(boost::beast::role_type::client));
    ws_.set_option(boost::beast::websocket::stream_base::decorator([](boost::beast::websocket::request_type& req) {
          req.set(boost::beast::http::field::user_agent, std::string(BOOST_BEAST_VERSION_STRING) + " websocket-client-async-ssl");
          }));
    ws_.async_handshake(host_, endpoint_, bind_executor(strand_, bind_front_handler(&Session::OnHandshake, shared_from_this())));
  }

  void Session::OnHandshake(boost::beast::error_code ec) {
    BOOST_LOG_TRIVIAL(trace) << "OnHandshake(" << ec.to_string() << ")";
    ws_.async_read(buffer_, bind_executor(strand_, bind_front_handler(&Session::OnRead, shared_from_this())));
  }

  void Session::OnRead(boost::beast::error_code ec, std::size_t bytes) {
    BOOST_LOG_TRIVIAL(trace) << "OnRead(" << ec.to_string() << "," << bytes << ")";
  }

}

