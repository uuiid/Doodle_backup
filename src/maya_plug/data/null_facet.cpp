//
// Created by TD on 2022/10/13.
//

#include "null_facet.h"

namespace doodle {
namespace maya_plug {
const std::string& null_facet::name() const noexcept {
  static std::string l_i{"null_facet"};
  return l_i;
}
void null_facet::operator()() {
  work_lock.emplace(boost::asio::make_work_guard(g_io_context()));
}
void null_facet::deconstruction() {
  work_lock.reset();
}
}  // namespace maya_plug
}  // namespace doodle