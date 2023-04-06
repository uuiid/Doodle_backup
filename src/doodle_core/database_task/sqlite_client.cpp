//
// Created by TD on 2022/6/2.
//

#include "sqlite_client.h"

#include <doodle_core/core/core_sig.h>
#include <doodle_core/core/core_sql.h>
#include <doodle_core/core/doodle_lib.h>
#include <doodle_core/gui_template/show_windows.h>
#include <doodle_core/metadata/work_task.h>
#include <doodle_core/thread_pool/process_message.h>

#include <boost/asio.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include "core/core_help_impl.h"
#include <core/core_set.h>
#include <core/status_info.h>
#include <database_task/delete_data.h>
#include <database_task/details/database.h>
#include <database_task/details/update_ctx.h>
#include <database_task/insert.h>
#include <database_task/select.h>
#include <database_task/update.h>
#include <metadata/metadata.h>
#include <range/v3/all.hpp>
#include <utility>

namespace doodle::database_n {

bsys::error_code file_translator::open_begin(const FSys::path& in_path) {
  doodle_lib::Get().ctx().get<database_info>().path_ = in_path;
  auto& k_msg                                        = g_reg()->ctx().emplace<process_message>();
  k_msg.set_name("加载数据");
  k_msg.set_state(k_msg.run);
  g_reg()->ctx().at<core_sig>().project_begin_open(in_path);
  is_opening = true;
  return {};
}
bsys::error_code file_translator::open(const FSys::path& in_path) {
  auto l_r = open_impl(in_path);
  return l_r;
}

bsys::error_code file_translator::open_end() {
  core_set::get_set().add_recent_project(doodle_lib::Get().ctx().get<database_info>().path_);
  g_reg()->ctx().at<core_sig>().project_end_open();
  auto& k_msg = g_reg()->ctx().emplace<process_message>();
  k_msg.set_name("完成写入数据");
  k_msg.set_state(k_msg.success);
  g_reg()->ctx().erase<process_message>();
  is_opening = false;
  return {};
}

bsys::error_code file_translator::save_begin(const FSys::path& in_path) {
  auto& k_msg = g_reg()->ctx().emplace<process_message>();
  k_msg.set_name("保存数据");
  k_msg.set_state(k_msg.run);
  g_reg()->ctx().at<core_sig>().save_begin();
  is_saving = true;
  return {};
}

bsys::error_code file_translator::save(const FSys::path& in_path) {
  auto l_r = save_impl(in_path);
  return l_r;
}

bsys::error_code file_translator::save_end() {
  g_reg()->ctx().at<status_info>().need_save = false;
  g_reg()->ctx().at<core_sig>().save_end();
  g_reg()->clear<data_status_save>();
  g_reg()->clear<data_status_delete>();
  auto& k_msg = g_reg()->ctx().emplace<process_message>();
  k_msg.set_name("完成写入数据");
  k_msg.set_state(k_msg.success);
  g_reg()->ctx().erase<process_message>();
  is_saving = false;
  return {};
}
void file_translator::new_file_scene(const FSys::path& in_path) {
  doodle_lib::Get().ctx().get<database_info>().path_ = in_path;
  auto& l_s                                          = g_reg()->ctx().emplace<status_info>();
  l_s.message                                        = "创建新项目";
  l_s.need_save                                      = true;
}

class sqlite_file::impl {
 public:
  registry_ptr registry_attr;
  bool error_retry{false};
  template <typename type_t>
  class impl_obs {
    entt::observer obs_update_;
    entt::observer obs_create_;

   public:
    explicit impl_obs(const registry_ptr& in_registry_ptr)
        : obs_update_(*in_registry_ptr, entt::collector.update<database>().where<type_t>()),
          obs_create_(*in_registry_ptr, entt::collector.group<database, type_t>()) {}

    void open(const registry_ptr& in_registry_ptr, conn_ptr& in_conn, std::map<std::int64_t, entt::entity>& in_handle) {
      obs_update_.disconnect();
      obs_create_.disconnect();
      database_n::sql_com<type_t>{in_registry_ptr}.select(in_conn, in_handle);
      obs_update_.connect(*in_registry_ptr, entt::collector.update<database>().where<type_t>());
      obs_create_.connect(*in_registry_ptr, entt::collector.group<database, type_t>());
    };

    void save(const registry_ptr& in_registry_ptr, conn_ptr& in_conn, const std::vector<std::int64_t>& in_handle) {
      database_n::sql_com<type_t> l_orm{in_registry_ptr};

      std::vector<entt::entity> l_create{};
      std::vector<entt::entity> l_update{};

      for (auto&& i : obs_create_) {
        if (in_registry_ptr->get<database>(i).is_install())
          l_create.emplace_back(i);
        else
          l_update.emplace_back(i);
      }
      for (auto&& i : obs_update_) {
        if (in_registry_ptr->get<database>(i).is_install())
          l_create.emplace_back(i);
        else
          l_update.emplace_back(i);
      }

      l_orm.insert(in_conn, l_create);
      l_orm.update(in_conn, l_update);
      l_orm.destroy(in_conn, in_handle);
      obs_update_.clear();
      obs_create_.clear();
    }

    ~impl_obs() {
      obs_create_.disconnect();
      obs_update_.disconnect();
    }
  };

  template <>
  class impl_obs<database> {
    entt::observer obs_update_;
    entt::observer obs_create_;
    std::vector<std::int64_t> destroy_ids_{};
    entt::connection conn_{};
    void on_destroy(entt::registry& in_reg, entt::entity in_entt) {
      if (auto& l_data = in_reg.get<database>(in_entt); l_data.is_install()) destroy_ids_.emplace_back(l_data.get_id());
    }

   public:
    explicit impl_obs(const registry_ptr& in_registry_ptr)
        : obs_update_(*in_registry_ptr, entt::collector.update<database>().where<database>()),
          obs_create_(*in_registry_ptr, entt::collector.group<database, database>()),
          destroy_ids_{},
          conn_{in_registry_ptr->on_destroy<database>().connect<&impl_obs<database>::on_destroy>(*this)} {}

    void open(const registry_ptr& in_registry_ptr, conn_ptr& in_conn, std::map<std::int64_t, entt::entity>& in_handle) {
      obs_update_.disconnect();
      obs_create_.disconnect();
      database_n::sql_com<database>{in_registry_ptr}.select(in_conn, in_handle);
      obs_update_.connect(*in_registry_ptr, entt::collector.update<database>().where<database>());
      obs_create_.connect(*in_registry_ptr, entt::collector.group<database, database>());
      destroy_ids_.clear();
    };

    void save(const registry_ptr& in_registry_ptr, conn_ptr& in_conn, std::vector<std::int64_t>& in_handle) {
      database_n::sql_com<database> l_orm{in_registry_ptr};

      std::vector<entt::entity> l_create{};
      std::vector<entt::entity> l_update{};

      for (auto&& i : obs_create_) {
        if (in_registry_ptr->get<database>(i).is_install())
          l_create.emplace_back(i);
        else
          l_update.emplace_back(i);
      }
      for (auto&& i : obs_update_) {
        if (in_registry_ptr->get<database>(i).is_install())
          l_create.emplace_back(i);
        else
          l_update.emplace_back(i);
      }
      in_handle = destroy_ids_;
      l_orm.insert(in_conn, l_create);
      l_orm.destroy(in_conn, destroy_ids_);
      destroy_ids_.clear();
      obs_update_.clear();
      obs_create_.clear();
    }

    ~impl_obs() {
      obs_create_.disconnect();
      obs_update_.disconnect();
      conn_.release();
    }
  };

  template <typename... arg>
  class obs_main {
    std::tuple<impl_obs<database>, arg...> obs_data_;

   public:
    explicit obs_main(const registry_ptr& in_registry_ptr)
        : obs_data_{std::make_tuple(impl_obs<database>{in_registry_ptr}, impl_obs<arg>{in_registry_ptr}...)} {}

    void open(const registry_ptr& in_registry_ptr, conn_ptr& in_conn) {
      std::map<std::int64_t, entt::entity> l_map{};

      std::apply([&](auto&&... x) { auto l_t = {(x.open(in_registry_ptr, in_conn, l_map), 0)...}; }, obs_data_);
    }

    void save(const registry_ptr& in_registry_ptr, conn_ptr& in_conn) {
      std::vector<std::int64_t> l_handles{};
      std::apply([&](auto&&... x) { auto l_t = {(x.save(in_registry_ptr, in_conn, l_handles), 0)...}; }, obs_data_);
    }
  };

  std::shared_ptr<boost::asio::system_timer> error_timer{};
};

sqlite_file::sqlite_file() : ptr(std::make_unique<impl>()) {}
sqlite_file::sqlite_file(registry_ptr in_registry) : ptr(std::make_unique<impl>()) {
  ptr->registry_attr = std::move(in_registry);
}
bsys::error_code sqlite_file::open_impl(const FSys::path& in_path) {
  ptr->registry_attr   = g_reg();
  constexpr auto l_loc = BOOST_CURRENT_LOCATION;
  if (!FSys::exists(in_path)) return bsys::error_code{error_enum::file_not_exists, &l_loc};

  database_n::select l_select{};
  auto l_k_con = doodle_lib::Get().ctx().get<database_info>().get_connection_const();
  l_select(*ptr->registry_attr, in_path, l_k_con);
  return {};
}
bsys::error_code sqlite_file::save_impl(const FSys::path& in_path) {
  ptr->registry_attr = g_reg();
  std::vector<entt::entity> delete_list;
  std::vector<entt::entity> install_list;
  std::vector<entt::entity> update_list;
  std::vector<entt::entity> next_delete_list;
  DOODLE_LOG_INFO("文件位置 {}", in_path);
  if (!FSys::exists(in_path) && in_path.generic_string() != ":memory:"s) {  /// \brief  不存在时直接保存所有的实体
    if (!FSys::exists(in_path.parent_path())) {
      FSys::create_directories(in_path.parent_path());
    }
    auto l_view  = g_reg()->view<doodle::database>();
    install_list = {l_view.begin(), l_view.end()};
  } else {  /// \brief   否则进行筛选

    auto l_dv = ptr->registry_attr->view<data_status_delete, database>();
    for (auto&& [e, d] : l_dv.each()) {
      if (d.is_install()) {
        delete_list.push_back(e);
      } else {
        next_delete_list.push_back(e);
      }
    }

    auto l_sv = ptr->registry_attr->view<data_status_save, database>();
    for (auto&& [e, d] : l_sv.each()) {
      if (d.is_install()) {
        update_list.push_back(e);
      } else {
        install_list.push_back(e);
      }
    }
  }

  try {
    auto l_k_con = doodle_lib::Get().ctx().get<database_info>().get_connection();
    auto l_tx    = sqlpp::start_transaction(*l_k_con);
    details::db_compatible::add_entity_table(*l_k_con);
    details::db_compatible::add_ctx_table(*l_k_con);
    details::db_compatible::add_component_table(*l_k_con);
    details::db_compatible::add_version_table(*l_k_con);
    details::db_compatible::delete_metadatatab_table(*l_k_con);
    if (delete_list.empty() && install_list.empty() && update_list.empty()) {
      /// \brief 只更新上下文
      database_n::details::update_ctx::ctx(*ptr->registry_attr, *l_k_con);
    } else {
      /// \brief 删除没有插入的
      ptr->registry_attr->destroy(next_delete_list.begin(), next_delete_list.end());
      if (!install_list.empty()) {
        database_n::insert l_sqlit_action{};
        l_sqlit_action(*ptr->registry_attr, install_list, l_k_con);
      }
      if (!update_list.empty()) {
        database_n::update_data l_sqlit_action{};
        l_sqlit_action(*ptr->registry_attr, update_list, l_k_con);
      }
      if (!delete_list.empty()) {
        database_n::delete_data l_sqlit_action{};
        l_sqlit_action(*ptr->registry_attr, delete_list, l_k_con);
      }
    }
    l_tx.commit();
  } catch (const sqlpp::exception& in_error) {
    DOODLE_LOG_INFO(boost::diagnostic_information(in_error));
    g_reg()->ctx().at<status_info>().message = "保存失败 3s 后重试";
    ptr->error_retry                         = true;
    ptr->error_timer                         = std::make_shared<boost::asio::system_timer>(g_io_context());
    ptr->error_timer->async_wait([l_path = in_path, this](auto&& in) {
      this->async_save(l_path, [](boost::system::error_code in) -> void {});
    });
    ptr->error_timer->expires_from_now(3s);
    //    return in_error;
  }

  return {};
}

sqlite_file::~sqlite_file()                                    = default;
sqlite_file::sqlite_file(sqlite_file&& in) noexcept            = default;
sqlite_file& sqlite_file::operator=(sqlite_file&& in) noexcept = default;

}  // namespace doodle::database_n
