//
// Created by TD on 2022/1/14.
//

#include "main_status_bar.h"

#include <doodle_core/core/status_info.h>
#include <doodle_core/thread_pool/process_message.h>

#include <boost/asio.hpp>

#include <lib_warp/imgui_warp.h>

/// \brief to https://github.com/ocornut/imgui/issues/1901
namespace ImGui {
bool BufferingBar(const char* label, float value, const ImVec2& size_arg, const ImU32& bg_col, const ImU32& fg_col) {
  ImGuiWindow* window = GetCurrentWindow();
  if (window->SkipItems) return false;

  ImGuiContext& g         = *GImGui;
  const ImGuiStyle& style = g.Style;
  const ImGuiID id        = window->GetID(label);

  ImVec2 pos              = window->DC.CursorPos;
  ImVec2 size             = size_arg;
  size.x -= style.FramePadding.x * 2;

  const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
  ItemSize(bb, style.FramePadding.y);
  if (!ItemAdd(bb, id)) return false;

  // Render
  const float circleStart = size.x * 0.7f;
  const float circleEnd   = size.x;
  const float circleWidth = circleEnd - circleStart;

  window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart, bb.Max.y), bg_col);
  window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart * value, bb.Max.y), fg_col);

  const float t     = g.Time;
  const float r     = size.y / 2;
  const float speed = 1.5f;

  const float a     = speed * 0;
  const float b     = speed * 0.333f;
  const float c     = speed * 0.666f;

  const float o1    = (circleWidth + r) * (t + a - speed * (int)((t + a) / speed)) / speed;
  const float o2    = (circleWidth + r) * (t + b - speed * (int)((t + b) / speed)) / speed;
  const float o3    = (circleWidth + r) * (t + c - speed * (int)((t + c) / speed)) / speed;

  window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o1, bb.Min.y + r), r, bg_col);
  window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o2, bb.Min.y + r), r, bg_col);
  window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o3, bb.Min.y + r), r, bg_col);
  return false;
}

bool Spinner(const char* label, float radius, int thickness, const ImU32& color) {
  ImGuiWindow* window = GetCurrentWindow();
  if (window->SkipItems) return false;

  ImGuiContext& g         = *GImGui;
  const ImGuiStyle& style = g.Style;
  const ImGuiID id        = window->GetID(label);

  ImVec2 pos              = window->DC.CursorPos;
  ImVec2 size((radius)*2, (radius + style.FramePadding.y) * 2);

  const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
  ItemSize(bb, style.FramePadding.y);
  if (!ItemAdd(bb, id)) return false;

  // Render
  window->DrawList->PathClear();

  int num_segments    = 30;
  int start           = abs(ImSin(g.Time * 1.8f) * (num_segments - 5));

  const float a_min   = IM_PI * 2.0f * ((float)start) / (float)num_segments;
  const float a_max   = IM_PI * 2.0f * ((float)num_segments - 3) / (float)num_segments;

  const ImVec2 centre = ImVec2(pos.x + radius, pos.y + radius + style.FramePadding.y);

  for (int i = 0; i < num_segments; i++) {
    const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
    window->DrawList->PathLineTo(
        ImVec2(centre.x + ImCos(a + g.Time * 8) * radius, centre.y + ImSin(a + g.Time * 8) * radius)
    );
  }

  window->DrawList->PathStroke(color, false, thickness);
  return false;
}
}  // namespace ImGui

namespace doodle::gui {
class main_status_bar::impl {
 public:
  std::shared_ptr<boost::asio::high_resolution_timer> timer{};
  bool call;
};

main_status_bar::main_status_bar() : p_i(std::make_unique<impl>()) { init(); }

void main_status_bar::init() { p_i->timer = std::make_shared<boost::asio::high_resolution_timer>(g_io_context()); }

bool main_status_bar::render() {
  dear::MenuBar{} && [&]() {
    /// \brief 渲染信息
    if (g_reg()->ctx().contains<status_info>()) {
      auto l_s = g_reg()->ctx().get<status_info>();
      if (l_s.need_save) {
        dear::Text("需要保存"s);
        ImGui::SameLine();
      }
      if (!l_s.message.empty()) {
        dear::Text(l_s.message);
        ImGui::SameLine();
      }
      dear::Text(fmt::format("{}/{}", l_s.select_size, l_s.show_size));
      ImGui::SameLine();
    }

    /// \brief 渲染进度条
    if (g_reg()->ctx().contains<process_message>()) {
      auto& l_msg = g_reg()->ctx().get<process_message>();
      if (!p_i->call && (l_msg.is_success() || l_msg.is_fail())) {
        p_i->call = true;
        p_i->timer->expires_after(1s);
        p_i->timer->async_wait([this](const boost::system::error_code& in_code) {
          p_i->call = false;
          if (in_code == boost::asio::error::operation_aborted) {
            return;
          }
          if (g_reg()->ctx().contains<process_message>()) {
            g_reg()->ctx().erase<process_message>();
          }
        });
      }

      dear::Text(l_msg.get_name());
      ImGui::SameLine();
      dear::Text(l_msg.message_back());
      ImGui::ProgressBar(
          boost::rational_cast<std::float_t>(l_msg.get_progress()), ImVec2{-FLT_MIN, 0.0f},
          fmt::format("{:04f}%", l_msg.get_progress_f()).c_str()
      );
    }
  };
  return true;
}

main_status_bar::main_status_bar(const main_status_bar& in) noexcept : p_i(std::make_unique<impl>(*in.p_i)) {}
main_status_bar::main_status_bar(main_status_bar&& in) noexcept { p_i = std::move(in.p_i); }
main_status_bar& main_status_bar::operator=(const main_status_bar& in) noexcept {
  *p_i = *p_i;
  return *this;
}
main_status_bar& main_status_bar::operator=(main_status_bar&& in) noexcept {
  p_i = std::move(in.p_i);
  return *this;
}

main_status_bar::~main_status_bar() = default;
}  // namespace doodle::gui
