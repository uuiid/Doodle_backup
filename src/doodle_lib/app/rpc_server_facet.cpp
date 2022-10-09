//
// Created by TD on 2022/10/8.
//

#include "rpc_server_facet.h"
#include <doodle_lib/json_rpc/json_rpc_server.h>
#include <doodle_core/json_rpc/core/server.h>
namespace doodle {
namespace facet {

class rpc_server_facet::impl {
 public:
  std::shared_ptr<json_rpc_server> rpc_server_attr;
  std::shared_ptr<json_rpc::server> server_attr;
  std::string name{"json_rpc"};
};

rpc_server_facet::rpc_server_facet()
    : p_i(std::make_unique<impl>()) {
  p_i->rpc_server_attr = std::make_shared<json_rpc_server>();
}
const std::string& rpc_server_facet::name() const noexcept {
  return p_i->name;
}
void rpc_server_facet::operator()() {
  p_i->server_attr = std::make_shared<json_rpc::server>(g_io_context(), 24445u);
  p_i->server_attr->set_rpc_server(p_i->rpc_server_attr);
}
void rpc_server_facet::deconstruction() {
  p_i->server_attr->stop();
  p_i->server_attr.reset();
  p_i->rpc_server_attr.reset();
}
std::shared_ptr<json_rpc::server> rpc_server_facet::server_attr() const {
  return p_i->server_attr;
}

rpc_server_facet::~rpc_server_facet() = default;
}  // namespace facet
}  // namespace doodle