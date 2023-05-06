﻿//
// Created by TD on 2021/6/17.
//

#include "doodle_lib.h"

#include <doodle_core/core/core_set.h>
#include <doodle_core/core/core_sig.h>
#include <doodle_core/core/init_register.h>
#include <doodle_core/core/program_info.h>
#include <doodle_core/database_task/sqlite_client.h>
#include <doodle_core/metadata/metadata_cpp.h>
#include <doodle_core/metadata/rules.h>

#include "core/doodle_lib.h"

#include <boost/locale.hpp>

#include <core/status_info.h>
#include <date/tz.h>
#include <exception/exception.h>
#include <logger/logger.h>
#include <memory>
#include <utility>

namespace doodle {

class doodle_lib::impl {
 public:
  std::shared_ptr<boost::asio::io_context> io_context_{};
  boost::asio::thread_pool thread_pool_attr{std::thread::hardware_concurrency() * 2};
  logger_ctr_ptr p_log{};
  registry_ptr reg{};

  inline static doodle_lib* self;
};

doodle_lib::doodle_lib() : ptr() {
  impl::self = this;
  init();
}

doodle_lib& doodle_lib::Get() { return *impl::self; }
void doodle_lib::init() {
  ptr              = std::move(std::make_unique<impl>());
  /// @brief 初始化其他
  ptr->io_context_ = std::make_shared<boost::asio::io_context>();
  ptr->p_log       = std::make_shared<logger_ctr_ptr::element_type>();
  ptr->reg         = std::make_shared<entt::registry>();

  init_register::instance().reg_class();

  //boost::locale::generator k_gen{};
  //k_gen.categories(
  //    boost::locale::all_categories ^ boost::locale::category_t::formatting ^ boost::locale::category_t::parsing
  //);
  //FSys::path::imbue(k_gen("zh_CN.UTF-8"));

  ctx().emplace<database_info>();
  ptr->reg->ctx().emplace<project>("C:/", "tmp_project");
  ptr->reg->ctx().emplace<project_config::base_config>();
  ptr->reg->ctx().emplace<user::current_user>();
  ptr->reg->ctx().emplace<core_sig>();

  ptr->reg->ctx().emplace<status_info>();
  ctx().emplace<database_n::file_translator_ptr>(std::make_shared<database_n::sqlite_file>(ptr->reg));
}

registry_ptr& doodle_lib::reg_attr() const { return ptr->reg; }

bool doodle_lib::operator==(const doodle_lib& in_rhs) const { return ptr == in_rhs.ptr; }

doodle_lib::~doodle_lib() = default;

registry_ptr& g_reg() { return doodle_lib::Get().ptr->reg; }
boost::asio::io_context& g_io_context() { return *doodle_lib::Get().ptr->io_context_; }
boost::asio::thread_pool& g_thread() { return doodle_lib::Get().ptr->thread_pool_attr; }
details::logger_ctrl& g_logger_ctrl() { return *doodle_lib::Get().ptr->p_log; }

}  // namespace doodle
