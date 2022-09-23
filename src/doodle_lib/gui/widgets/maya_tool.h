//
// Created by TD on 2022/9/21.
//

#pragma once

#include <doodle_lib/doodle_lib_fwd.h>
#include <doodle_core/core/init_register.h>
#include <doodle_lib/gui/gui_ref/base_window.h>

namespace doodle::gui {

class DOODLELIB_API maya_tool
    : public gui::base_windows<
          dear::Begin,
          maya_tool>{
  FSys::path p_cloth_path;
  std::string p_text;
  std::vector<FSys::path> p_sim_path;
  bool p_only_sim;
  bool p_sim_export_fbx;
  bool p_sim_only_export;

  bool p_use_all_ref;
  bool p_upload_files;
  std::string title_name_;

 public:
  maya_tool();

  constexpr static std::string_view name{gui::config::menu_w::comm_maya_tool};

  void init();
  const std::string& title() const override;
  void render();
};

}  // namespace doodle::gui