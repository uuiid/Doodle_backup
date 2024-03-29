//
// Created by TD on 2022/8/4.
//

#include "time_point_info.h"

#include <tuple>

namespace doodle::business::rules_ns {

bool time_point_info::operator==(const time_point_info& in) const {
  return std::tie(first, second, info) == std::tie(in.first, in.second, in.info);
}

void to_json(nlohmann::json& j, const time_point_info& p) {
  j["first"]         = p.first;
  j["second"]        = p.second;
  j["info"]          = p.info;
  j["is_extra_work"] = p.is_extra_work;
}
void from_json(const nlohmann::json& j, time_point_info& p) {
  j["first"].get_to(p.first);
  j["second"].get_to(p.second);
  j["info"].get_to(p.info);
  if (j.contains("is_extra_work")) j["is_extra_work"].get_to(p.is_extra_work);
}
}  // namespace doodle::business::rules_ns
