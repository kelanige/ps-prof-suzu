#include <memory>
#include <string>

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>

namespace Pokemon {
  class Session : public std::enable_shared_from_this<Session> {
    public:
      explicit Session(boost::asio::io_context& ioc, boost::asio::ssl::context& ctx);

      void Run(const std::string& host, const std::string& port, const std::string& endpoint);

    private:
      boost::asio::io_context& ioc_;
  };
}

