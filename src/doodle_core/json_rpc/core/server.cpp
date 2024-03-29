#include "server.h"

#include <doodle_core/logger/logger.h>

#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>

#include <iostream>
#include <json_rpc/core/parser_rpc.h>
#include <json_rpc/core/rpc_server.h>
#include <json_rpc/core/session.h>
#include <nlohmann/json.hpp>
#include <utility>

namespace doodle::json_rpc {
class server::impl {
 public:
  explicit impl(boost::asio::io_context &in_io_context, std::uint16_t in_port)
      : io_context_(in_io_context),
        acceptor_(
            in_io_context, boost::asio::ip::tcp::endpoint{boost::asio::ip::address::from_string("127.0.0.1"), in_port}
        ),
        rpc_server_ptr_() {}

  boost::asio::io_context &io_context_;
  boost::asio::ip::tcp::acceptor acceptor_;
  std::shared_ptr<session_manager> session_manager_ptr;
  std::shared_ptr<rpc_server> rpc_server_ptr_;
};

server::server(boost::asio::io_context &in_io_context, std::uint16_t in_port)
    : ptr(std::make_unique<impl>(in_io_context, in_port)) {}
void server::do_accept() {
  ptr->acceptor_.async_accept([this](boost::system::error_code in_err, boost::asio::ip::tcp::socket in_socket) {
    if (!in_err) {
      auto l_session = std::make_shared<session>(ptr->io_context_, std::move(in_socket));

      this->do_accept();
    } else {
      DOODLE_LOG_INFO(in_err.message());
    }
  });
}
void server::set_rpc_server(const std::shared_ptr<rpc_server> &in_server) {
  ptr->rpc_server_ptr_ = in_server;
  do_accept();
}
void server::stop() { ptr->acceptor_.cancel(); }
std::uint16_t server::get_prot() const { return ptr->acceptor_.local_endpoint().port(); }
server::~server()                             = default;
server::server(server &&) noexcept            = default;
server &server::operator=(server &&) noexcept = default;

}  // namespace doodle::json_rpc
