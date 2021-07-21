//
// Created by TD on 2021/7/21.
//

#pragma once
#include <DoodleLib/DoodleLib_fwd.h>
#include <DoodleLib/Gui/action/action.h>
namespace doodle {
namespace action_arg {

class DOODLELIB_API arg_excel : public arg_path {
 public:
  using time_point = std::chrono::time_point<std::chrono::system_clock>;
  std::pair<time_point, time_point> p_time_range;
};

}  // namespace action_arg

class DOODLELIB_API actn_export_excel : public action_indirect<action_arg::arg_excel> {
  void export_excel();
  void find_parent(const MetadataPtr& in_ptr);
  std::map<std::uint64_t, MetadataPtr> p_list;

 public:
  actn_export_excel();

  bool is_async() override;
  long_term_ptr run(const MetadataPtr& in_data, const MetadataPtr& in_parent) override;
};

}  // namespace doodle
