//
// Created by TD on 2021/11/22.
//

#pragma once

#include <doodle_lib/doodle_lib_fwd.h>
#include <doodle_lib/exception/exception.h>
#include <maya/MGlobal.h>
#include <maya/MStatus.h>
#include <maya/MString.h>

namespace doodle {
class maya_error : public doodle_error {
 public:
  explicit maya_error(const std::string& err)
      : doodle_error(err){};
  explicit maya_error(const MString& in_m_string)
      : doodle_error(in_m_string.asUTF8()){};
  explicit maya_error(const MStatus& in_status)
      : maya_error(in_status.errorString()){};
};
}  // namespace doodle

namespace doodle::maya_plug {
class play_blast;
using play_blast_ptr = std::shared_ptr<play_blast>;

class d_str {
 public:
  string p_u8_str{};
  template <class MStr, std::enable_if_t<std::is_same_v<MString, MStr>, bool> = true>
  explicit d_str(const MStr& in)
      : p_u8_str(in.asUTF8()){};

  //  explicit d_str(const MString& in)
  //      : p_u8_str(in.asUTF8()){};

  d_str(const string& in_u8_str)
      : p_u8_str(in_u8_str) {
  }

  inline operator MString() const {
    MString k_r{};
    k_r.setUTF8(p_u8_str.c_str());
    return k_r;
  }
  inline operator string() const {
    return p_u8_str;
  }
  inline string str() const {
    return p_u8_str;
  }
};

// inline void chick_maya_status(const MStatus& in_status) {
//   if (in_status != MStatus::MStatusCode::kSuccess) {
//     const MString& l_string = in_status.errorString();
//     MGlobal::displayError(l_string);
//     DOODLE_LOG_ERROR(l_string.asUTF8());
//     throw doodle_error{l_string.asUTF8()};
//   }
// };

#define DOODLE_CHICK(in_status)                        \
  {                                                    \
    if (in_status != MStatus::MStatusCode::kSuccess) { \
      DOODLE_LOG_ERROR(in_status.errorString());       \
      throw maya_error{in_status};                     \
    }                                                  \
  };

}  // namespace doodle::maya_plug

namespace fmt {
template <>
struct fmt::formatter<::doodle::maya_error> : fmt::formatter<fmt::string_view> {
  template <typename FormatContext>
  auto format(const ::doodle::maya_error& in_, FormatContext& ctx) -> decltype(ctx.out()) {
    return formatter<string_view>::format(
        in_.what(),
        ctx);
  }
};

template <>
struct fmt::formatter<MString> : fmt::formatter<fmt::string_view> {
  template <typename FormatContext>
  auto format(const MString& in_, FormatContext& ctx) -> decltype(ctx.out()) {
    std::string k_str = in_.asUTF8();
    return formatter<string_view>::format(
        k_str,
        ctx);
  }
};

}  // namespace fmt
