//
// Created by TD on 2021/9/19.
//

#pragma once

#include <DoodleLib/DoodleLib_fwd.h>
#include <DoodleLib/Gui/action/command.h>
namespace doodle {
class DOODLELIB_API comm_export_fbx : public command_tool {
  std::vector<FSys::path> p_files;

 public:
  comm_export_fbx();
  bool is_async() override;
  bool run() override;
};

class DOODLELIB_API comm_qcloth_sim : public command_tool {
  FSys::path p_cloth_path;
  std::shared_ptr<std::string> p_text;
  std::vector<FSys::path> p_sim_path;
  bool p_only_sim;

 public:
  comm_qcloth_sim();
  bool is_async() override;
  bool run() override;
};

class DOODLELIB_API comm_create_video : public command_tool {
  struct image_paths {
    std::vector<FSys::path> p_path_list;
    FSys::path p_out_path;
    std::string p_show_name;
    bool use_dir;
  };
  std::vector<FSys::path> p_video_path;
  std::vector<image_paths> p_image_path;
  std::shared_ptr<std::string> p_out_path;

 public:
  comm_create_video();
  bool is_async() override;
  bool run() override;
};

class DOODLELIB_API comm_import_ue_files : public command_tool {
 public:
  comm_import_ue_files();
  bool is_async() override;
  bool run() override;
};

class DOODLELIB_API comm_create_ue_project : public command_tool {
 public:
  comm_create_ue_project();
  bool is_async() override;
  bool run() override;
};
}  // namespace doodle
