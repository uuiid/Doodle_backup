//
// Created by TD on 2021/12/8.
//

#pragma once
#include <doodle_lib/doodle_lib_fwd.h>
#include <spdlog/sinks/base_sink.h>

#include <maya/MGlobal.h>
namespace doodle::maya_plug {
template <class Mutex>
class maya_msg : public spdlog::sinks::base_sink<Mutex> {
 public:
  maya_msg() = default;

 protected:
  void sink_it_(const spdlog::details::log_msg &msg) override {
    spdlog::memory_buf_t formatted;
    spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);
    auto k_str{fmt::to_string(formatted)};
    k_str.pop_back();
    MString k_m_str{d_str{k_str}};
    std::cout << k_m_str << "\n";
    switch (msg.level) {
      case spdlog::level::err: {
        MGlobal::displayError(k_m_str);
        break;
      }
      case spdlog::level::warn: {
        MGlobal::displayWarning(k_m_str);
        break;
      }
      default: {
        MGlobal::displayInfo(k_m_str);
        break;
      }
    }
  }

  void flush_() override {
    std::cout.flush();
  }
};
using maya_msg_mt = maya_msg<std::mutex>;
}  // namespace doodle::maya_plug
