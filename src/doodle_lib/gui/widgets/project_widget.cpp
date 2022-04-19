//
// Created by TD on 2021/9/16.
//

#include "project_widget.h"

#include <doodle_lib/core/doodle_lib.h>
#include <doodle_lib/app/app.h>
#include <doodle_lib/core/program_options.h>
#include <doodle_lib/lib_warp/imgui_warp.h>
#include <doodle_lib/metadata/metadata_cpp.h>
#include <doodle_lib/core/init_register.h>

namespace doodle {

project_widget::project_widget()
    : p_c() {
  title_name_ = std::string{name};
  show_       = true;

}

project_widget::~project_widget() = default;

void project_widget::init() {
  gui::window_panel::init();
}


void project_widget::render() {
  dear::Table{"project", 3} && [this]() {
    imgui::TableSetupColumn("名称");
    imgui::TableSetupColumn("路径");
    imgui::TableSetupColumn("字母名称");
    imgui::TableHeadersRow();

    bool k_chick{false};

    for (const auto&& [e, l_p] : g_reg()->view<project>().each()) {
      auto k_h = make_handle(e);
      imgui::TableNextRow();
      imgui::TableNextColumn();
      if (dear::Selectable(l_p.show_str(),
                           k_h == p_c,
                           ImGuiSelectableFlags_SpanAllColumns)) {
        p_c     = k_h;
        k_chick = true;
      }
      imgui::TableNextColumn();
      dear::Text(l_p.get_path().generic_string());
      imgui::TableNextColumn();
      dear::Text(l_p.str());
    }
    if (k_chick) {
      auto k_reg = g_reg();

      k_reg->ctx().erase<root_ref>();
      k_reg->ctx().emplace<root_ref>(p_c);
    }
  };
}



}  // namespace doodle
