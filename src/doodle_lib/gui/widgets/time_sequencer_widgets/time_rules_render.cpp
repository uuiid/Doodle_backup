//
// Created by TD on 2022/8/4.
//

#include "time_rules_render.h"

#include "doodle_core/doodle_core_fwd.h"
#include <doodle_core/metadata/detail/time_point_info.h>
#include <doodle_core/metadata/rules.h>
#include <doodle_core/metadata/time_point_wrap.h>
#include <doodle_core/metadata/user.h>

#include <doodle_app/gui/base/modify_guard.h>
#include <doodle_app/gui/base/ref_base.h>
#include <doodle_app/lib_warp/imgui_warp.h>

#include "boost/lambda2.hpp"
#include <boost/asio.hpp>
#include <boost/asio/post.hpp>
#include <boost/lambda2/lambda2.hpp>
#include <boost/operators.hpp>
#include <boost/optional.hpp>
#include <boost/signals2/connection.hpp>

#include "gui/widgets/time_sequencer_widgets/time_rules_render.h"
#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <fmt/compile.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <functional>
#include <imgui.h>
#include <imgui_internal.h>
#include <range/v3/action/erase.hpp>
#include <range/v3/action/remove_if.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/algorithm/for_each.hpp>
#include <range/v3/algorithm/for_each_n.hpp>
#include <range/v3/algorithm/remove_if.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/transform.hpp>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace doodle::gui::time_sequencer_widget_ns {

namespace {
class work_gui_data {
 public:
  using base_type   = gui_cache<std::array<gui_cache<bool>, 7>>;

  using friend_type = business::rules::work_day_type;

  base_type gui_attr;
  work_gui_data() : work_gui_data(business::rules::work_Monday_to_Friday){};
  explicit work_gui_data(const friend_type& in_work_day_type)
      : gui_attr(
            "工作周"s,
            std::array<gui::gui_cache<bool>, 7>{
                gui::gui_cache<bool>{"星期日"s, in_work_day_type[0]},
                gui::gui_cache<bool>{"星期一"s, in_work_day_type[1]},
                gui::gui_cache<bool>{"星期二"s, in_work_day_type[2]},
                gui::gui_cache<bool>{"星期三"s, in_work_day_type[3]},
                gui::gui_cache<bool>{"星期四"s, in_work_day_type[4]},
                gui::gui_cache<bool>{"星期五"s, in_work_day_type[5]},
                gui::gui_cache<bool>{"星期六"s, in_work_day_type[6]}}
        ){};

  explicit operator friend_type() const {
    friend_type l_r{};
    l_r[0] = gui_attr.data[0]();
    l_r[1] = gui_attr.data[1]();
    l_r[2] = gui_attr.data[2]();
    l_r[3] = gui_attr.data[3]();
    l_r[4] = gui_attr.data[4]();
    l_r[5] = gui_attr.data[5]();
    l_r[6] = gui_attr.data[6]();
    return l_r;
  }
};

class time_hh_mm_ss_gui_data : public gui_cache<std::array<std::int32_t, 3>> {
 public:
  using base_type   = gui_cache<std::array<std::int32_t, 3>>;

  using friend_type = chrono::seconds;
  time_hh_mm_ss_gui_data() : base_type("时分秒", std::array<std::int32_t, 3>{}){};

  explicit time_hh_mm_ss_gui_data(const friend_type& in_seconds) : time_hh_mm_ss_gui_data() {
    chrono::hh_mm_ss l_hh_mm_ss{in_seconds};
    data[0] = boost::numeric_cast<std::int32_t>(l_hh_mm_ss.hours().count());
    data[1] = boost::numeric_cast<std::int32_t>(l_hh_mm_ss.minutes().count());
  };

  explicit operator friend_type() const {
    return chrono::hours{data[0]} + chrono::minutes{data[1]} + chrono::seconds{data[2]};
  }
};
class time_yy_mm_dd_gui_data : public gui_cache<std::array<std::int32_t, 3>> {
 public:
  using base_type = gui_cache<std::array<std::int32_t, 3>>;

  time_yy_mm_dd_gui_data() : base_type("年月日"s, std::array<std::int32_t, 3>{}){};
  explicit time_yy_mm_dd_gui_data(const chrono::local_days& in_days) : time_yy_mm_dd_gui_data() {
    chrono::year_month_day l_year_month_day{in_days};
    data[0] = boost::numeric_cast<std::int32_t>((std::int32_t)l_year_month_day.year());
    data[1] = boost::numeric_cast<std::int32_t>((std::uint32_t)l_year_month_day.month());
    data[2] = boost::numeric_cast<std::int32_t>((std::uint32_t)l_year_month_day.day());
  };

  explicit operator chrono::local_days() const {
    return chrono::local_days{chrono::year_month_day{
        chrono::year{data[0]}, chrono::month{boost::numeric_cast<std::uint32_t>(data[1])},
        chrono::day{boost::numeric_cast<std::uint32_t>(data[2])}}};
  }
};

class time_warp_gui_data : boost::equality_comparable<time_warp_gui_data> {
 public:
  using friend_type = time_point_wrap;
  time_yy_mm_dd_gui_data ymd{};
  time_hh_mm_ss_gui_data hms{};

  time_warp_gui_data() : time_warp_gui_data(friend_type{}){};

  explicit time_warp_gui_data(const friend_type& in_point_wrap) : time_warp_gui_data("时间"s, in_point_wrap){};

  explicit time_warp_gui_data(const std::string& in_string, const friend_type& in_point_wrap) {
    auto&& [l_y, l_s] = in_point_wrap.compose_1();
    ymd               = time_yy_mm_dd_gui_data{l_y};
    hms               = time_hh_mm_ss_gui_data{l_s};
  };
  explicit operator friend_type() const { return friend_type{chrono::local_days{ymd} + chrono::seconds{hms}}; }
  bool operator==(const time_warp_gui_data& in) const { return std::tie(ymd, hms) == std::tie(in.ymd, in.hms); }
};

class time_info_gui_data : boost::equality_comparable<time_info_gui_data> {
 public:
  time_warp_gui_data begin_time;
  time_warp_gui_data end_time;
  gui_cache<std::string> info;
  gui_cache_name_id delete_node{"删除"s};

  using friend_type = ::doodle::business::rules::point_type;

  time_info_gui_data() : time_info_gui_data(friend_type{}) {}
  explicit time_info_gui_data(const friend_type& in_point_type)
      : begin_time(), end_time(), info("备注"s, in_point_type.info) {
    begin_time = time_warp_gui_data{"开始时间"s, in_point_type.first};
    end_time   = time_warp_gui_data{"结束时间"s, in_point_type.second};
  };

  explicit operator friend_type() const {
    return friend_type{time_point_wrap{begin_time}, time_point_wrap{end_time}, info()};
  }

  bool operator==(const time_info_gui_data& in) const {
    return std::tie(begin_time, end_time, info, delete_node) ==
           std::tie(in.begin_time, in.end_time, in.info, in.delete_node);
  }
};

class time_work_gui_data : boost::equality_comparable<time_work_gui_data> {
 public:
  time_hh_mm_ss_gui_data begin{};
  time_hh_mm_ss_gui_data end{};
  gui_cache_name_id name_id_delete{"删除"s};
  time_work_gui_data() = default;

  using friend_type    = std::pair<chrono::seconds, chrono::seconds>;

  explicit time_work_gui_data(const friend_type& in_pair) : begin(in_pair.first), end(in_pair.second) {}

  explicit operator friend_type() const {
    return std::make_pair(friend_type::first_type{begin}, friend_type::second_type{end});
  }
  bool operator==(const time_work_gui_data& in_rhs) const {
    return begin == in_rhs.begin && end == in_rhs.end && name_id_delete == in_rhs.name_id_delete;
  }
};

class work_gui_data_render {
 public:
  using gui_data_type = work_gui_data;
  modify_guard modify_guard_{};
  gui_data_type gui_data{};
  bool render() {
    modify_guard_.begin_flag();
    {
      ranges::for_each(gui_data.gui_attr(), [this](decltype(gui_data.gui_attr().front()) in_value) {
        modify_guard_ = ImGui::Checkbox(*in_value, &in_value);
        if (in_value.gui_name != gui_data.gui_attr().back().gui_name) ImGui::SameLine();
      });
    }
    return modify_guard_;
  }
  [[nodiscard]] gui_data_type::friend_type get() const { return gui_data_type::friend_type{gui_data}; }

  void set(const gui_data_type::friend_type& in_type) { gui_data = gui_data_type{in_type}; }
};

class time_work_gui_data_render : boost::equality_comparable<time_work_gui_data_render> {
 public:
  using friend_type = std::pair<chrono::seconds, chrono::seconds>;
  time_hh_mm_ss_gui_data begin{};
  time_hh_mm_ss_gui_data end{};

  gui_cache_name_id name_id{"每日工作时间"};
  gui_cache_name_id name_id_add{"添加"s};
  gui_cache_name_id name_id_delete{"删除"s};
  modify_guard modify_guard_;

  bool render() {
    modify_guard_.begin_flag();

    dear::ItemWidth{ImGui::GetCurrentWindow()->WorkRect.GetSize().x / 3} && [&]() {
      modify_guard_ = ImGui::SliderInt3(*begin, begin.data.data(), 0, 59);
      ImGui::SameLine();
      auto l_size = ImGui::GetItemRectSize();
      l_size.x /= 5;
      l_size.y /= 5;
      ImGui::Dummy(l_size);
      ImGui::SameLine();

      modify_guard_ = ImGui::SliderInt3(*end, end.data.data(), 0, 59);
    };

    return modify_guard_;
  };

  [[nodiscard]] friend_type get() const {
    return {time_hh_mm_ss_gui_data::friend_type{begin}, time_hh_mm_ss_gui_data::friend_type{end}};
  }

  void set(const friend_type& in_type) {
    begin = time_hh_mm_ss_gui_data{in_type.first};
    end   = time_hh_mm_ss_gui_data{in_type.second};
  }
  bool operator==(const time_work_gui_data_render& in_rhs) const {
    return begin == in_rhs.begin && end == in_rhs.end && name_id_delete == in_rhs.name_id_delete;
  }
};

class time_info_gui_data_render : boost::equality_comparable<time_info_gui_data_render> {
 public:
  std::string show_str{};
  business::rules::time_point_info data{};
  gui_cache_name_id edit_buiion{"编辑"};
  gui_cache_name_id fulfil{"完成"};
  gui_cache_name_id delete_buttton{"删除"};
  bool use_edit{};

  using friend_type = time_info_gui_data::friend_type;
  time_warp_gui_data begin_time{};
  time_warp_gui_data end_time{};
  gui_cache<std::string> info{};

  gui_cache_name_id gui_name{};

  modify_guard modify_guard_{};

  bool render() {
    modify_guard_.begin_flag();

    dear::Text(show_str);
    ImGui::SameLine();
    if (ImGui::Button(*edit_buiion)) use_edit = true;

    if (use_edit)
      dear::ItemWidth{ImGui::GetCurrentWindow()->WorkRect.GetSize().x / 4} && [&]() {
        dear::Text("开始时间"s);
        ImGui::SameLine();
        modify_guard_ = ImGui::InputInt3(*begin_time.ymd.gui_name, begin_time.ymd.data.data());
        ImGui::SameLine();
        modify_guard_ = ImGui::InputInt3(*begin_time.hms.gui_name, begin_time.hms.data.data());

        dear::Text("结束时间"s);
        ImGui::SameLine();
        modify_guard_ = ImGui::InputInt3(*end_time.ymd.gui_name, end_time.ymd.data.data());
        ImGui::SameLine();
        modify_guard_ = ImGui::InputInt3(*end_time.hms.gui_name, end_time.hms.data.data());

        modify_guard_ = ImGui::InputText(*info, &info);
        ImGui::SameLine();
        if (ImGui::Button(*fulfil)) use_edit = false;
      };
    if (modify_guard_) show_str = fmt::to_string(get());

    return modify_guard_;
  }

  void set(const friend_type& in_type) {
    show_str   = fmt::to_string(in_type);

    begin_time = time_warp_gui_data{in_type.first};
    end_time   = time_warp_gui_data{in_type.second};
    info       = in_type.info;
  }
  [[nodiscard]] friend_type get() const {
    return {time_warp_gui_data::friend_type{begin_time}, time_warp_gui_data::friend_type{end_time}, std::string{info}};
  }

  bool operator==(const time_info_gui_data_render& in_rhs) const {
    return begin_time == in_rhs.begin_time && end_time == in_rhs.end_time && delete_buttton == in_rhs.delete_buttton;
  }
};

}  // namespace

class time_rules_render::impl {
 public:
  constexpr const static std::array<std::string_view, 7> work_show{"星期日", "星期一", "星期二", "星期三",
                                                                   "星期四", "星期五", "星期六"};

  rules_type rules_attr;

  std::vector<boost::signals2::scoped_connection> sig_scoped;
  std::string show_str{};

  gui_cache_name_id setting{"高级设置"};

  work_gui_data_render work_gui_data_attr{};
  std::vector<time_work_gui_data_render> time_work_gui_data_attr{};
  gui_cache_name_id time_work_gui_data_attr_add_button{"添加"s};
  std::vector<std::vector<time_work_gui_data_render>> absolute_deduction_attr{};
  std::vector<time_info_gui_data_render> extra_work_attr{};
  gui_cache_name_id extra_work_attr_add_button{"添加"s};
  std::vector<time_info_gui_data_render> extra_rest_attr{};
  gui_cache_name_id extra_rest_attr_add_button{"添加"s};
};

time_rules_render::time_rules_render() : p_i(std::make_unique<impl>()) {
  p_i->rules_attr = rules_type::get_default();
  rules_attr(p_i->rules_attr);
  // p_i->render_time.extra_work_attr.gui_name = gui_cache_name_id{"添加加班时间"s};
  // p_i->render_time.extra_rest_attr.gui_name = gui_cache_name_id{"添加调休时间"s};

  p_i->sig_scoped.emplace_back(g_reg()->ctx().at<core_sig>().project_end_open.connect([this]() {
    this->rules_attr(g_reg()->ctx().at<user::current_user>().get_handle().get<rules_type>());
  }));

  p_i->sig_scoped.emplace_back(g_reg()->ctx().at<core_sig>().select_handles.connect([this](auto) {
    this->rules_attr(g_reg()->ctx().at<user::current_user>().get_handle().get<rules_type>());
  }));
}

void time_rules_render::print_show_str() {
  p_i->show_str = fmt::format(
      "每周工作日 {}\n每天工作时间 {}\n排除时间(必然会被扣除):\n{}\n(节假日会自动扣除)",
      fmt::join(
          ranges::views::ints(0, 7) | ranges::views::filter([&](std::int32_t in_index) {
            return p_i->rules_attr.work_weekdays()[in_index];
          }) | ranges::views::transform([&](std::int32_t in_index) { return std::string{p_i->work_show[in_index]}; }),
          " "
      ),
      fmt::join(
          p_i->rules_attr.work_time() |
              ranges::views::transform([&](const std::decay_t<decltype(p_i->rules_attr.work_time())>::value_type& in) {
                return fmt::format("{:%H:%M} 到 {:%H:%M}", in.first, in.second);
              }),
          ","
      ),
      fmt::join(
          ranges::views::ints(0, 7) | ranges::views::transform([&](std::int32_t in_index) {
            return fmt::format(
                "{} {}", p_i->work_show[in_index],
                fmt::join(
                    p_i->rules_attr.absolute_deduction[in_index] |
                        ranges::views::transform(
                            [](const std::decay_t<decltype(p_i->rules_attr.absolute_deduction[in_index])>::value_type&
                                   in) {
                              return fmt::format("{:%H:%M} 到 {:%H:%M}", in.first, in.second);
                              // return std::make_pair(in.first, in.second);
                            }
                        ),
                    ","
                )
            );
          }),
          "\n"
      )
  );
}

const time_rules_render::rules_type& time_rules_render::rules_attr() const { return p_i->rules_attr; }
void time_rules_render::rules_attr(const time_rules_render::rules_type& in_rules_type) {
  p_i->rules_attr = in_rules_type;

  p_i->work_gui_data_attr.set(in_rules_type.work_weekdays());
  p_i->time_work_gui_data_attr = in_rules_type.work_time() |
                                 ranges::views::transform(
                                     [&](const decltype(in_rules_type.work_pair_p)::value_type& in
                                     ) -> decltype(p_i->time_work_gui_data_attr)::value_type {
                                       decltype(p_i->time_work_gui_data_attr)::value_type l_a{};
                                       l_a.set(in);
                                       return l_a;
                                     }
                                 ) |
                                 ranges::to_vector;

  p_i->extra_work_attr = in_rules_type.extra_work() |
                         ranges::views::transform(
                             [&](const decltype(in_rules_type.extra_work_p)::value_type& in
                             ) -> decltype(p_i->extra_work_attr)::value_type {
                               decltype(p_i->extra_work_attr)::value_type l_a{};
                               l_a.set(in);
                               return l_a;
                             }
                         ) |
                         ranges::to_vector;
  p_i->extra_rest_attr = in_rules_type.extra_rest() |
                         ranges::views::transform(
                             [&](const decltype(in_rules_type.extra_rest_p)::value_type& in
                             ) -> decltype(p_i->extra_rest_attr)::value_type {
                               decltype(p_i->extra_rest_attr)::value_type l_a{};
                               l_a.set(in);
                               return l_a;
                             }
                         ) |
                         ranges::to_vector;
  print_show_str();
}
bool time_rules_render::render() {
  modify_guard_.begin_flag();

  dear::Text(p_i->show_str);

  dear::TreeNode{*p_i->setting} && [&]() {
    if (p_i->work_gui_data_attr.render()) {
      p_i->rules_attr.work_weekdays_p = p_i->work_gui_data_attr.get();
      modify_guard_.modifyed();
    };

    if (ImGui::Button(*p_i->time_work_gui_data_attr_add_button)) {
      p_i->time_work_gui_data_attr.emplace_back().set(p_i->rules_attr.work_pair_p.emplace_back());
      modify_guard_.modifyed();
    }

    ranges::for_each(ranges::views::ints(0ull, p_i->time_work_gui_data_attr.size()), [&](std::size_t in_index) {
      auto& l_r = p_i->time_work_gui_data_attr[in_index];
      if (l_r.render()) {
        p_i->rules_attr.work_pair_p[in_index] = l_r.get();
        modify_guard_.modifyed();
      }
      ImGui::SameLine();
      if (ImGui::Button(*l_r.name_id_delete)) {
        boost::asio::post(g_io_context(), [this, in_index]() {
          p_i->time_work_gui_data_attr |=
              ranges::actions::remove_if(boost::lambda2::_1 == p_i->time_work_gui_data_attr[in_index]);

          p_i->rules_attr.work_pair_p |=
              ranges::actions::remove_if(boost::lambda2::_1 == p_i->rules_attr.work_pair_p[in_index]);
          modify_guard_.modifyed();
        });
      }
    });
  };
  ImGui::Text("加班时间");
  ImGui::SameLine();
  if (ImGui::Button(*p_i->extra_work_attr_add_button)) {
    p_i->extra_work_attr.emplace_back().set(p_i->rules_attr.extra_work_p.emplace_back());
    modify_guard_.modifyed();
  }
  ranges::for_each(ranges::views::ints(0ull, p_i->extra_work_attr.size()), [&](std::size_t in) {
    auto& l_r = p_i->extra_work_attr[in];
    if (l_r.render()) {
      p_i->rules_attr.extra_work_p[in] = l_r.get();
      modify_guard_.modifyed();
    }

    ImGui::SameLine();

    if (ImGui::Button(*l_r.delete_buttton)) {
      boost::asio::post(g_io_context(), [this, in]() {
        p_i->extra_work_attr |= ranges::actions::remove_if(boost::lambda2::_1 == p_i->extra_work_attr[in]);
        p_i->rules_attr.extra_work_p |=
            ranges::actions::remove_if(boost::lambda2::_1 == p_i->rules_attr.extra_work_p[in]);
        modify_guard_.modifyed();
      });
    }
  });

  ImGui::Text("调休时间");
  ImGui::SameLine();
  if (ImGui::Button(*p_i->extra_rest_attr_add_button)) {
    p_i->extra_rest_attr.emplace_back().set(p_i->rules_attr.extra_rest_p.emplace_back());
    modify_guard_.modifyed();
  }
  ranges::for_each(ranges::views::ints(0ull, p_i->extra_rest_attr.size()), [&](std::size_t in) {
    auto& l_r = p_i->extra_rest_attr[in];
    if (l_r.render()) {
      p_i->rules_attr.extra_rest_p[in] = l_r.get();
      modify_guard_.modifyed();
    }

    ImGui::SameLine();

    if (ImGui::Button(*l_r.delete_buttton)) {
      boost::asio::post(g_io_context(), [this, in]() {
        p_i->extra_rest_attr |= ranges::actions::remove_if(boost::lambda2::_1 == p_i->extra_rest_attr[in]);
        p_i->rules_attr.extra_rest_p |=
            ranges::actions::remove_if(boost::lambda2::_1 == p_i->rules_attr.extra_rest_p[in]);
        modify_guard_.modifyed();
      });
    }
  });

  if (modify_guard_) print_show_str();

  return modify_guard_;
}
time_rules_render::~time_rules_render() = default;
}  // namespace doodle::gui::time_sequencer_widget_ns
