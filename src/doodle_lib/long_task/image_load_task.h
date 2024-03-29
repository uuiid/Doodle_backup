//
// Created by TD on 2022/2/25.
//

#pragma once

#include <doodle_lib/doodle_lib_fwd.h>

#include <boost/asio/basic_io_object.hpp>
#include <boost/asio/io_context.hpp>

#include <memory>

namespace doodle {

class DOODLELIB_API image_load_task {
 private:
  class impl;
  std::unique_ptr<impl> p_i;
  using call_ptr = std::shared_ptr<std::function<void()>>;
  void read_image(const entt::handle& in_handle, const call_ptr& in_call);

 public:
  explicit image_load_task();
  virtual ~image_load_task();

  template <typename CompletionHandler>
  auto async_read(const entt::handle& in_handle, CompletionHandler&& in_completion) {
    in_handle.template any_of<image_icon>()
        ? void()
        : throw_error(
              error_enum::component_missing_error, fmt::format("在加载图片时缺失 image_icon 组件 {}", in_handle)
          );
    using l_call = std::function<void()>;
    return boost::asio::async_initiate<CompletionHandler, void()>(
        [this, in_handle](auto&& in_completion_handler) {
          auto l_f = std::make_shared<l_call>(std::forward<decltype(in_completion_handler)>(in_completion_handler));
          boost::asio::post(g_io_context(), [this, l_f, in_handle]() {
            this->read_image(in_handle, l_f);
          });
        },
        in_completion
    );
  }
};

}  // namespace doodle
