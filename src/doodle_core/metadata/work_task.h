#pragma once

#include "doodle_core/metadata/detail/user_ref.h"
#include "doodle_core/metadata/metadata.h"
#include "doodle_core/metadata/time_point_wrap.h"
#include "doodle_core_fwd.h"

#include <entt/entity/fwd.hpp>
#include <string>

namespace doodle {
/**
 * @brief 任务类
 *
 */
class DOODLE_CORE_API work_task_info {
 public:
  /// 任务人引用
  user_ref user_ref;
  /// 时间
  time_point_wrap time;
  /// 名称
  std::string task_name;
  /// 地点
  std::string region;
  /// 概述
  std::string abstract;

 private:
  friend void DOODLE_CORE_API to_json(nlohmann::json& j, const work_task_info& p);
  friend void DOODLE_CORE_API from_json(const nlohmann::json& j, work_task_info& p);
};
}  // namespace doodle