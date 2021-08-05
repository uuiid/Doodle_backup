//
// Created by TD on 2021/8/4.
//

#include "tool_box_menu_factory.h"

#include <Exception/Exception.h>
#include <Gui/action/actn_tool_box.h>
#include <Gui/progress.h>

#include <nana/gui/msgbox.hpp>
#include <nana/gui/filebox.hpp>

namespace doodle {

tool_box_menu_factory::tool_box_menu_factory(nana::window in_window)
    : p_window(in_window),
      p_list() {
}

void tool_box_menu_factory::create_menu() {
  auto k_export_maya = std::make_shared<toolbox::actn_export_maya>();
  p_list.push_back(k_export_maya);
  auto k_get_paths = [this]() {
    nana::filebox k_filebox{p_window, true};
    k_filebox.allow_multi_select(true);

    toolbox::actn_export_maya::arg k_arg{};
    k_arg.date      = k_filebox();
    k_arg.is_cancel = k_arg.date.empty();
    return k_arg;
  };
  k_export_maya->sig_get_arg.connect(k_get_paths);

  auto k_create_video = std::make_shared<toolbox::actn_create_video>();
  p_list.push_back(k_create_video);
  k_create_video->sig_get_arg.connect(k_get_paths);

  auto k_connect_video = std::make_shared<toolbox::actn_connect_video>();
  p_list.push_back(k_connect_video);
  k_connect_video->sig_get_arg.connect(k_get_paths);

  auto k_ue4 = std::make_shared<toolbox::actn_ue4_shot_episodes>();
  p_list.push_back(k_ue4);
  k_ue4->sig_get_arg.connect([this]() {
    nana::filebox k_filebox{p_window, true};
    k_filebox.allow_multi_select(false);

    toolbox::actn_ue4_shot_episodes::arg k_arg{};
    k_arg.date      = k_filebox().front();
    k_arg.is_cancel = k_arg.date.empty();
    return k_arg;
  });
}
void tool_box_menu_factory::operator()(nana::menu& in_menu) {
  create_menu();
  auto k_f = shared_from_this();
  for (auto& k_i : p_list) {
    if (k_i)
      in_menu.append(
          k_i->class_name(),
          [k_i, k_f](const nana::menu::item_proxy&) {
            try {
              (*k_i)();
              if (k_i->is_async()) {
                auto k_long = k_i->get_long_term_signal();
                if (k_long)
                  progress::create_progress(k_f->p_window, k_long, "结果");
              }
            } catch (DoodleError& error) {
              DOODLE_LOG_WARN(error.what())
              nana::msgbox k_msgbox{k_f->p_window, error.what()};
              k_msgbox.show();
            }
          });
    else
      in_menu.append_splitter();
  }
}
}  // namespace doodle
