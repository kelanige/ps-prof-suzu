#include <memory>
#include <string>

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/beast/ssl.hpp>

namespace Pokemon {
  class Session : public std::enable_shared_from_this<Session> {
    public:
      explicit Session(boost::asio::io_context& ioc, boost::asio::ssl::context& ctx);

      void Run(const std::string& host, const std::string& port, const std::string& endpoint);
      void OnResolve(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type results);
      void OnConnect(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type::endpoint_type ep);
      void OnSSLHandshake(boost::beast::error_code ec);
      void OnHandshake(boost::beast::error_code ec);
      void OnRead(boost::beast::error_code ec, std::size_t bytes);

    private:
      boost::asio::io_context& ioc_;
      boost::asio::ip::tcp::resolver resolver_;
      boost::beast::websocket::stream<boost::beast::ssl_stream<boost::beast::tcp_stream>> ws_;
      boost::beast::flat_buffer buffer_;
      boost::asio::strand<boost::asio::io_context::executor_type> strand_;
      std::string host_;
      std::string endpoint_;
  };
}

