//
// Created by TD on 2022/9/13.
//

#pragma once
#include <doodle_dingding/doodle_dingding_fwd.h>

#include <nlohmann/json_fwd.hpp>
#include <nlohmann/json.hpp>
namespace doodle::dingding {

class DOODLE_DINGDING_API department_query {
 public:
  std::int32_t dept_id{1};
  std::string language{"zh_CN"};

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(department_query, dept_id, language)
};

class DOODLE_DINGDING_API department {
 public:
  department() = default;
  std::int32_t dept_id{};
  std::string name{};
  std::int32_t parent_id{};
  bool create_dept_group{};
  bool auto_add_user{};
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(department, dept_id, name, parent_id, create_dept_group, auto_add_user)
};

}  // namespace doodle::dingding