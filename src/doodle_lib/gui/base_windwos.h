//
// Created by TD on 2021/9/15.
//

#pragma once
#include <doodle_lib/doodle_lib_fwd.h>
#include <doodle_lib/lib_warp/imgui_warp.h>

#include <boost/hana/experimental/printable.hpp>
#include <boost/type_erasure/any.hpp>
#include <boost/type_erasure/free.hpp>
#include <boost/type_erasure/member.hpp>

namespace doodle {

class DOODLELIB_API base_widget
    : public details::no_copy,
      public std::enable_shared_from_this<base_widget> {
 protected:
  string p_class_name;

  virtual bool use_register() { return true; };

  template <class in_class>
  bool render_tmp(registry_ptr& in) {
    auto k_v = in->view<in_class>();
    bool k_{true};
    for (auto k_i : k_v) {
      dear::TreeNode{k_v.get<in_class>(k_i).class_name().c_str()} && [&] {
        k_ &= k_v.get<in_class>(k_i).render();
      };
    }
    return true;
  }

  template <class... arg>
  bool render_() {
    auto k_reg = g_reg();
    return (render_tmp<arg>(k_reg) && ...);
  }

 public:
  virtual void post_constructor();
  virtual void frame_render() = 0;
  virtual const string& get_class_name() const;
};

class DOODLELIB_API windows_warp_base : public base_widget {
 public:
  bool_ptr p_show;
  windows_warp_base(bool init_show = false)
      : p_show(new_object<bool>(init_show)){};
  virtual ~windows_warp_base() {
    *p_show = false;
  }
  virtual bool load_show()       = 0;
  virtual void save_show() const = 0;
};

template <class widget>
class DOODLELIB_API windows_warp : public windows_warp_base {
 public:
  using widget_ptr = std::shared_ptr<widget>;

  widget_ptr p_widget;

  windows_warp(bool init_show = false)
      : windows_warp_base(init_show),
        p_widget(new_object<widget>()){};

  void frame_render() override {
    if (*p_show) {
      dear::Begin{
          this->p_widget->get_class_name().c_str(),
          p_show.get()} &&
          std::bind(&widget::frame_render, this->p_widget.get());
    }
  }
  const string& get_class_name() const override {
    return this->p_widget->get_class_name();
  }

  bool load_show() override {
    auto& set = core_set::getSet();
    if (set.widget_show.count(this->get_class_name()) > 0) {
      *p_show = set.widget_show[this->get_class_name()];
      return true;
    }
    return false;
  }
  void save_show() const override {
    auto& set                               = core_set::getSet();
    set.widget_show[this->get_class_name()] = *p_show;
  }
};

template <class widget>
std::shared_ptr<windows_warp<widget>> win_warp_cast(const base_widget_ptr& in) {
  return std::dynamic_pointer_cast<windows_warp<widget>>(in);
}

template <class widget>
std::shared_ptr<widget> win_cast(const base_widget_ptr& in) {
  auto ptr = win_warp_cast<widget>(in);
  if (ptr)
    return ptr->p_widget;
  else
    return nullptr;
}
BOOST_TYPE_ERASURE_MEMBER(render);

using widget_ = boost::type_erasure::any<
    boost::mpl::vector<
        has_render<bool()>,
        boost::type_erasure::copy_constructible<>,
        boost::type_erasure::typeid_<>,
        boost::type_erasure::relaxed>>;

}  // namespace doodle
