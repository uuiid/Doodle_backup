//
// Created by TD on 2022/1/14.
//

#pragma once

#include <doodle_app/doodle_app_fwd.h>
#include <doodle_app/gui/base/base_window.h>

namespace doodle::gui {
class DOODLE_APP_API main_status_bar {
 private:
  class impl;
  std::unique_ptr<impl> p_i;

 public:
  main_status_bar();
  virtual ~main_status_bar();

  void init();
  main_status_bar(const main_status_bar& in) noexcept;
  main_status_bar(main_status_bar&& in) noexcept;
  main_status_bar& operator=(const main_status_bar& in) noexcept;
  main_status_bar& operator=(main_status_bar&& in) noexcept;

  static constexpr std::int32_t flags{
      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar};
  static constexpr std::string_view name{"状态栏_main"};

  [[maybe_unused]] bool render();
};
}  // namespace doodle::gui
