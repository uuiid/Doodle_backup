//
// Created by TD on 2022/1/20.
//

#include "get_input_dialog.h"

#include <doodle_core/core/app_facet.h>
#include <doodle_core/database_task/sqlite_client.h>
#include <doodle_core/metadata/metadata.h>
#include <doodle_core/metadata/project.h>

#include <doodle_app/app/app_command.h>
#include <doodle_app/app/facet/gui_facet.h>
#include <doodle_app/gui/open_file_dialog.h>

#include "imgui.h"
#include <gui/base/ref_base.h>
#include <lib_warp/imgui_warp.h>
#include <utility>

namespace doodle::gui {

class create_project_dialog::impl {
 public:
  project prj{"C:/", "tmp"s};

  FSys::path path;
  std::string path_gui;
  std::string name{"tmp"s};
  std::shared_ptr<FSys::path> in_path;
  gui_cache<entt::handle> select_button_id{"选择文件夹", make_handle()};

  std::string title{"输入项目"s};
};

bool create_project_dialog::render() {
  dear::Text(fmt::format("路径: {}", p_i->path_gui));

  if (ImGui::Button(*p_i->select_button_id)) {
    g_windows_manage().create_windows_arg(
        windows_init_arg{}
            .create<file_dialog>(file_dialog::dialog_args{}.set_use_dir().async_read([this](const FSys::path& in) {
              p_i->path     = in / (p_i->prj.p_name + std::string{doodle_config::doodle_db_name});
              p_i->path_gui = p_i->path.generic_string();
            }))
            .set_title("选择文件夹")
            .set_render_type<dear::Popup>()

    );
  }

  if (dear::InputText("名称", &(p_i->name))) {
    p_i->prj.set_name(p_i->name);
    p_i->path.remove_filename();
    p_i->path /= (p_i->prj.p_name + std::string{doodle_config::doodle_db_name});
    p_i->path_gui = p_i->path.generic_string();
  }

  if (imgui::Button("ok")) {
    p_i->prj.set_path(p_i->path.parent_path());
    p_i->select_button_id().destroy();
    doodle_lib::Get().ctx().get<database_n::file_translator_ptr>()->new_file_scene(p_i->path);
    g_reg()->ctx().get<project>() = p_i->prj;
    ImGui::CloseCurrentPopup();
    return false;
  }
  return true;
}

create_project_dialog::create_project_dialog() : p_i(std::make_unique<impl>()) {
  p_i->path     = core_set::get_set().get_doc();
  p_i->path_gui = p_i->path.generic_string();
}
create_project_dialog::~create_project_dialog() = default;

const std::string& create_project_dialog::title() const { return p_i->title; }

class close_exit_dialog::impl {
 public:
  std::string title{"退出"s};
};
bool close_exit_dialog::render() {
  auto l_text_size = ImGui::CalcTextSize("是否退出?");
  ImGui::SetCursorPosX(ImGui::GetCursorPosX() + l_text_size.x);
  ImGui::SetCursorPosY(ImGui::GetCursorPosY() + l_text_size.y * 2);
  //  ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
  ImGui::Text("是否退出?");
  ImGui::SameLine();
  ImGui::Dummy(l_text_size);

  ImGui::SetCursorPosY(ImGui::GetCursorPosY() + l_text_size.y);
  ImGui::SetCursorPosX(ImGui::GetCursorPosX() + l_text_size.x / 2);
  if (ImGui::Button("yes")) {
    ImGui::CloseCurrentPopup();
    quit();
  }
  ImGui::SameLine();
  ImGui::Dummy({l_text_size.x, l_text_size.y});
  ImGui::SameLine();
  if (ImGui::Button("no")) {
    ImGui::CloseCurrentPopup();
  }
  ImGui::Dummy(l_text_size);
  ImGui::Dummy(l_text_size);
  return open;
}
close_exit_dialog::close_exit_dialog() : p_i(std::make_unique<impl>()) {}
close_exit_dialog::close_exit_dialog(const quit_slot_type& in) : close_exit_dialog() { quit.connect(in); }

close_exit_dialog::~close_exit_dialog() = default;
const std::string& close_exit_dialog::title() const { return p_i->title; }

}  // namespace doodle::gui
