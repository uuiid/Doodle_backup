//
// Created by TD on 2022/6/2.
//

#pragma once

#include <doodle_core/doodle_core_fwd.h>
#include <doodle_core/logger/logger.h>

#include <boost/asio/async_result.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>

#include <filesystem>
#include <utility>
namespace doodle::database_n {

class file_translator;
using file_translator_ptr = std::shared_ptr<file_translator>;
class DOODLE_CORE_API file_translator : public std::enable_shared_from_this<file_translator> {
 private:
  bsys::error_code open_begin();
  bsys::error_code open();
  bsys::error_code open_end();

  bsys::error_code save_begin();
  bsys::error_code save();
  bsys::error_code save_end();

  bool is_saving{};
  bool is_opening{};

 protected:
  /**
   * @brief  文件加载(@b 非主线程) 可以阻塞,
   * @param in_path 传入的保存路径
   * @return 错误代码(异步)
   */
  virtual bsys::error_code open_impl() = 0;
  /**
   * @brief 文件保存(@b 非主线程) 可以阻塞,
   * @param in_path 传入的需要保存的路径
   * @return 错误代码(异步)
   */
  virtual bsys::error_code save_impl() = 0;

  enum class state : std::uint8_t { init, end };

  FSys::path project_path;

 public:
  virtual ~file_translator() = default;
  /**
   * @brief 使用路径打开项目文件
   * @param in_path 传入的项目文件路径
   */
  template <typename CompletionToken>
  auto async_open(const FSys::path& in_path, CompletionToken&& token) ->
      typename boost::asio::async_result<typename std::decay_t<CompletionToken>, void(bsys::error_code)>::return_type {
    project_path = in_path;
    return boost::asio::async_initiate<CompletionToken, void(bsys::error_code)>(
        [l_s = this->shared_from_this()](auto&& completion_handler) {
          if (l_s->is_opening) return;
          l_s->open_begin();

          std::function<void(bsys::error_code)> call{completion_handler};
          boost::asio::post(g_thread(), [l_s, call]() {
            auto l_r = l_s->open();
            boost::asio::post(g_io_context(), [call, l_r, l_s]() {
              l_s->open_end();
              call(l_r);
            });
          });
        },
        token
    );
  };

  /**
   * @brief 使用路径打开项目文件
   * @param in_path 传入的项目文件路径
   */
  template <typename CompletionToken>
  auto async_save(const FSys::path& in_path, CompletionToken&& token) ->
      typename boost::asio::async_result<typename std::decay_t<CompletionToken>, void(bsys::error_code)>::return_type {
    project_path = in_path;
    return boost::asio::async_initiate<CompletionToken, void(bsys::error_code)>(
        [l_s = this->shared_from_this()](auto&& completion_handler) {
          if (l_s->is_saving) return;
          l_s->save_begin();

          std::function<void(bsys::error_code)> call{completion_handler};
          boost::asio::post(g_thread(), [l_s, call]() {
            auto l_r = l_s->save();
            boost::asio::post(g_io_context(), [call, l_r, l_s]() {
              l_s->save_end();
              call(l_r);
            });
          });
        },
        token
    );
  };

  inline void save_(const FSys::path& in_path) {
    if (is_saving) return;
    project_path = in_path;
    save_begin();
    save();
    save_end();
  }
  inline void open_(const FSys::path& in_path) {
    if (is_opening) return;
    project_path = in_path;
    open_begin();
    open();
    open_end();
  }

  virtual void new_file_scene(const FSys::path& in_path);
};

class DOODLE_CORE_API sqlite_file : public file_translator {
 private:
  class impl;
  std::unique_ptr<impl> ptr;

 protected:
  bsys::error_code open_impl() override;
  bsys::error_code save_impl() override;

 public:
  sqlite_file();
  explicit sqlite_file(registry_ptr in_registry);
  virtual ~sqlite_file();

  sqlite_file(const sqlite_file& in) noexcept            = delete;
  sqlite_file& operator=(const sqlite_file& in) noexcept = delete;

  sqlite_file(sqlite_file&& in) noexcept;
  sqlite_file& operator=(sqlite_file&& in) noexcept;
};

}  // namespace doodle::database_n
