//
// Created by TD on 2021/12/13.
//

#pragma once
#include <maya_plug/main/maya_plug_fwd.h>

namespace doodle::maya_plug {
namespace {
constexpr char create_ref_file_command_name[] = "doodle_create_ref_file";
constexpr char ref_file_load_command_name[]   = "doodle_ref_file_load";
constexpr char ref_file_sim_command_name[]    = "doodle_ref_file_sim";
constexpr char ref_file_export_command_name[] = "doodle_ref_file_export";
constexpr char load_project_name[]            = "doodle_load_project";
constexpr char set_cloth_cache_path_name[]    = "doodle_set_cloth_cache_path";
}  // namespace



MSyntax ref_file_export_syntax();
MSyntax set_cloth_cache_path_syntax();

/**
 * @brief 导出文件中需要导出的集合体(abc或者fbx)
 */
class ref_file_export_command
    : public TemplateAction<ref_file_export_command, ref_file_export_command_name, ref_file_export_syntax> {
 public:
  enum export_type : std::uint32_t { abc = 0, fbx = 1 };
  MStatus doIt(const MArgList&) override;
};
/**
 * @brief 打开并加载文件
 */

class set_cloth_cache_path
    : public TemplateAction<set_cloth_cache_path, set_cloth_cache_path_name, set_cloth_cache_path_syntax> {
 public:
  MStatus doIt(const MArgList&) override;
};
}  // namespace doodle::maya_plug
