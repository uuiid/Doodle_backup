//
// Created by TD on 2021/9/23.
//

#pragma once

#include <doodle_lib/doodle_lib_fwd.h>
#include <doodle_lib/gui/gui_ref/base_window.h>

#include <boost/signals2.hpp>
namespace doodle {

namespace edit_widgets_ns {
class edit_assets_data {
 public:
  FSys::path old_path;
  std::string new_name;
};

}  // namespace edit_widgets_ns

/**
 * @brief 各种编辑组件和创建句柄的窗口
 *
 * 在没有计算出文件路径的时候, 其中,名称为空
 *
 */
class DOODLELIB_API edit_widgets
    : public gui::window_panel {
  class impl;
  std::unique_ptr<impl> p_i;

  void edit_handle();
  void add_handle();
  void clear_handle();
  void notify_file_list() const;

 public:
  /**
   * @brief Construct a new edit widgets object
   *
   */
  edit_widgets();
  /**
   * @brief Destroy the edit widgets object
   *
   */
  ~edit_widgets() override;
  /**
   * @brief 窗口显示名称
   *
   */
  constexpr static std::string_view name{gui::config::menu_w::edit_};

  /**
   * @brief 初始化窗口
   *
   * 连接获取选择信号 获取 文件列表中的选择物体(单选)
   * 开始打开信号  在项目打开时清除获取的选中句柄, 并禁用编辑
   * 结束打开信号 启用编辑
   *
   *
   */
  void init();

  /**
   * @brief 失败结束任务
   *
   */
  void failed();

  /**
   * @brief 每帧刷新函数
   *
   * @param data 自定义数据
   */
  void render() override;
};

namespace edit_widgets_ns {
constexpr auto init = []() {
  entt::meta<edit_widgets>()
      .type()
      .prop("name"_hs, std::string{edit_widgets::name})
      .base<gui::window_panel>();
};
class init_class
    : public init_register::registrar_lambda<init, 3> {};

}  // namespace edit_widgets_ns
}  // namespace doodle