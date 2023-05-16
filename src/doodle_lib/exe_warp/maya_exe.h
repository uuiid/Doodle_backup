//
// Created by TD on 2021/12/25.
//

#pragma once

#include <doodle_lib/doodle_lib_fwd.h>

#include <boost/asio/async_result.hpp>
#include <boost/asio/thread_pool.hpp>

#include <bitset>

namespace doodle::maya_exe_ns {
namespace flags {
static constexpr std::bitset<8> k_replace_ref_file{0b1 << 0};
static constexpr std::bitset<8> k_sim_file{0b1 << 1};
static constexpr std::bitset<8> k_export_abc_type{0b1 << 2};
static constexpr std::bitset<8> k_export_fbx_type{0b1 << 3};
static constexpr std::bitset<8> k_create_play_blast{0b1 << 4};
// static constexpr std::bitset<8> create_ref_file{0b1 << 0};

}  // namespace flags

class arg {
 public:
  arg()          = default;
  virtual ~arg() = default;
  FSys::path file_path{};
  FSys::path project_{};
  std::int32_t t_post{};
  std::int32_t export_anim_time{};
  std::bitset<8> bitset_;
  friend void to_json(nlohmann::json &in_nlohmann_json_j, const arg &in_nlohmann_json_t) {
    in_nlohmann_json_j["path"]             = in_nlohmann_json_t.file_path.generic_string();
    in_nlohmann_json_j["project_"]         = in_nlohmann_json_t.project_.generic_string();
    in_nlohmann_json_j["t_post"]           = in_nlohmann_json_t.t_post;
    in_nlohmann_json_j["export_anim_time"] = in_nlohmann_json_t.export_anim_time;
    in_nlohmann_json_j["bitset_"]          = in_nlohmann_json_t.bitset_;
  }
  friend void from_json(const nlohmann::json &in_nlohmann_json_j, arg &in_nlohmann_json_t) {
    in_nlohmann_json_j["path"].get_to(in_nlohmann_json_t.file_path);
    in_nlohmann_json_j["project_"].get_to(in_nlohmann_json_t.project_);
    in_nlohmann_json_j["t_post"].get_to(in_nlohmann_json_t.t_post);
    in_nlohmann_json_j["export_anim_time"].get_to(in_nlohmann_json_t.export_anim_time);
    in_nlohmann_json_j["bitset_"].get_to(in_nlohmann_json_t.bitset_);
  }
};

class DOODLELIB_API qcloth_arg : public maya_exe_ns::arg {
 public:
  [[nodiscard]] std::string to_str() const;

  friend void to_json(nlohmann::json &nlohmann_json_j, const qcloth_arg &nlohmann_json_t) {
    to_json(nlohmann_json_j, dynamic_cast<const arg &>(nlohmann_json_t));
  };
  friend void from_json(const nlohmann::json &nlohmann_json_j, qcloth_arg &nlohmann_json_t) {
    from_json(nlohmann_json_j, dynamic_cast<arg &>(nlohmann_json_t));
  };
};

class DOODLELIB_API export_fbx_arg : public maya_exe_ns::arg {
 public:
  bool use_all_ref;
  bool upload_file;

  [[nodiscard]] std::string to_str() const;

  friend void to_json(nlohmann::json &nlohmann_json_j, const export_fbx_arg &nlohmann_json_t) {
    to_json(nlohmann_json_j, dynamic_cast<const arg &>(nlohmann_json_t));
    nlohmann_json_j["use_all_ref"] = nlohmann_json_t.use_all_ref;
    nlohmann_json_j["upload_file"] = nlohmann_json_t.upload_file;
  };
};

class DOODLELIB_API replace_file_arg : public maya_exe_ns::arg {
 public:
  bool replace_file_all;
  std::vector<entt::handle> save_handle{};

  [[nodiscard]] std::string to_str() const;

  friend void to_json(nlohmann::json &nlohmann_json_j, const replace_file_arg &nlohmann_json_t) {
    to_json(nlohmann_json_j, dynamic_cast<const arg &>(nlohmann_json_t));
    nlohmann_json_j["replace_file_all"] = nlohmann_json_t.replace_file_all;
  };
};

class DOODLELIB_API clear_file_arg : public maya_exe_ns::arg {
 public:
  std::string save_file_extension_attr;

  [[nodiscard]] std::string to_str() const;

  friend void to_json(nlohmann::json &nlohmann_json_j, const clear_file_arg &nlohmann_json_t) {
    to_json(nlohmann_json_j, dynamic_cast<const arg &>(nlohmann_json_t));
    nlohmann_json_j["replace_file_all"] = nlohmann_json_t.save_file_extension_attr;
  };
};

}  // namespace doodle::maya_exe_ns
namespace doodle {
class DOODLELIB_API maya_exe {
  class impl;
  std::unique_ptr<impl> p_i;
  static void add_maya_fun_tool();
  void notify_run();
  using call_fun_type = std::function<void(boost::system::error_code)>;
  void queue_up(
      const entt::handle &in_msg, const std::string &in_string, const std::shared_ptr<call_fun_type> &in_call_fun,
      const FSys::path &in_run_path
  );

 public:
  maya_exe();

  virtual ~maya_exe();

  template <typename CompletionHandler, typename Arg_t>
  auto async_run_maya(const entt::handle &in_handle, const Arg_t &in_arg, CompletionHandler &&in_completion) {
    auto l_msg_ref = in_handle.get_or_emplace<process_message>();
    l_msg_ref.set_name(in_arg.file_path.filename().generic_string());
    return boost::asio::async_initiate<CompletionHandler, void(boost::system::error_code)>(
        [this, l_msg_ref, &in_arg, in_handle](auto &&in_completion_handler) {
          auto l_fun =
              std::make_shared<call_fun_type>(std::forward<decltype(in_completion_handler)>(in_completion_handler));
          this->queue_up(in_handle, in_arg.to_str(), l_fun, in_arg.file_path);
        },
        in_completion
    );
  };
};
using maya_exe_ptr = std::shared_ptr<maya_exe>;
}  // namespace doodle
