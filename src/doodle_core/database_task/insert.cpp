//
// Created by TD on 2022/5/30.
//

#include "insert.h"

#include <doodle_core/core/core_sql.h>
#include <doodle_core/core/doodle_lib.h>
#include <doodle_core/database_task/details/update_ctx.h>
#include <doodle_core/database_task/sql_file.h>
#include <doodle_core/logger/logger.h>
#include <doodle_core/metadata/detail/time_point_info.h>
#include <doodle_core/metadata/image_icon.h>
#include <doodle_core/metadata/importance.h>
#include <doodle_core/metadata/metadata_cpp.h>
#include <doodle_core/metadata/redirection_path_info.h>
#include <doodle_core/metadata/rules.h>
#include <doodle_core/metadata/user.h>
#include <doodle_core/metadata/work_task.h>
#include <doodle_core/thread_pool/process_message.h>

#include <boost/asio.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <database_task/details/com_data.h>
#include <range/v3/all.hpp>
#include <sqlpp11/sqlite3/sqlite3.h>
#include <sqlpp11/sqlpp11.h>

namespace doodle::database_n {
namespace {

/**
 * @brief 实体数据
 */
class entity_data {
 public:
  entt::entity entt_{};
  std::uint64_t l_id{};
  std::string uuid_data{};
};
}  // namespace
class insert::impl {
 public:
  /// @brief 线程未来数据获取
  std::vector<std::future<void>> futures_{};
  using com_data = details::com_data;
  /// @brief 传入的实体列表
  std::vector<entt::entity> entt_list{};
  /// @brief 实体数据生成
  std::map<entt::entity, std::shared_ptr<entity_data>> main_tabls{};
  /// @brief 组件数据生成
  std::vector<com_data> com_tabls{};

  std::vector<std::pair<std::int32_t, std::string>> ctx_tabls{};

  using boost_strand = boost::asio::strand<boost::asio::thread_pool::executor_type>;

  /// @brief boost 无锁保护
  boost_strand strand_{boost::asio::make_strand(g_thread())};
  ///@brief 原子停止指示
  std::atomic_bool stop{false};

  ///@brief 线程未来数据获取
  std::future<void> future_{};
  std::size_t size{};

  static void create_db(sqlpp::sqlite3::connection &in_db) {
    details::db_compatible::add_entity_table(in_db);
    details::db_compatible::add_ctx_table(in_db);
    details::db_compatible::add_component_table(in_db);
    details::db_compatible::add_version_table(in_db);
    details::db_compatible::delete_metadatatab_table(in_db);
  }

  /**
   * @brief 在注册表中插入实体
   * @param in_db 传入的插入数据库连接
   */
  void insert_db_entity(sqlpp::sqlite3::connection &in_db) {}
  /**
   * @brief 在数据库中插入组件
   * @param in_db 传入的插入数据库连接
   */
  void insert_db_com(sqlpp::sqlite3::connection &in_db) {}

  void set_database_id() {
    ranges::for_each(entt_list, [this](entt::entity &in_) {
      entt::handle l_h{*g_reg(), in_};
      l_h.get<database>().set_id(main_tabls[in_]->l_id);
    });
  }
  /**
   * @brief 创建实体数据(多线程)
   */
  void create_entt_data() {
    main_tabls = entt_list | ranges::views::transform([](const entt::entity &in) {
                   auto l_i   = std::make_shared<entity_data>();
                   l_i->entt_ = in;
                   return std::make_pair(in, l_i);
                 }) |
                 ranges::to<std::map<entt::entity, std::shared_ptr<entity_data>>>();
    ranges::for_each(main_tabls, [this](decltype(main_tabls)::value_type &in) {
      if (stop) return;
      futures_.emplace_back(boost::asio::post(g_thread(), std::packaged_task<void()>{[=]() {
                                                if (stop) return;
                                                auto l_h = entt::handle{*g_reg(), in.second->entt_};
                                                in.second->uuid_data =
                                                    boost::uuids::to_string(l_h.get<database>().uuid());
                                                g_reg()->ctx().emplace<process_message>().progress_step({1, size * 4});
                                              }}));
    });
  }

  // #define Type_T doodle::project
  /**
   * @brief 创建组件数据
   * @tparam Type_T 组件类型
   */
  template <typename Type_T>
  void _create_com_data_(std::size_t in_size) {
    ranges::for_each(entt_list, [this, in_size](const entt::entity &in) {
      if (stop) return;
      futures_.emplace_back(
          boost::asio::post(strand_, std::packaged_task<void()>{[=]() {
                              if (stop) return;
                              auto l_h = entt::handle{*g_reg(), in};
                              if (l_h.all_of<Type_T>()) {
                                nlohmann::json l_j{};
                                l_j = l_h.get<Type_T>();
                                com_tabls.emplace_back(in, entt::type_id<Type_T>().hash(), l_j.dump());
                              }
                              g_reg()->ctx().emplace<process_message>().progress_step({1, size * in_size * 4});
                            }})
      );
    });
  }
  template <typename... Type_T>
  void create_com_data() {
    auto l_size = sizeof...(Type_T);
    (_create_com_data_<Type_T>(l_size), ...);
  }
};

insert::insert() : p_i(std::make_unique<impl>()) {}

insert::~insert() = default;

void insert::operator()(
    const entt::registry &in_registry, const std::vector<entt::entity> &in_insert_data, conn_ptr &in_connect
) {
  p_i->entt_list = in_insert_data;
  p_i->size      = p_i->entt_list.size();
  g_reg()->ctx().emplace<process_message>().message("创建实体数据");
  p_i->create_entt_data();
  g_reg()->ctx().emplace<process_message>().message("组件数据...");
#include "details/macro.h"
  p_i->create_com_data<DOODLE_SQLITE_TYPE>();
  g_reg()->ctx().emplace<process_message>().message("完成数据线程准备");
  for (auto &f : p_i->futures_) {
    if (p_i->stop) return;
    f.get();
  }
  g_reg()->ctx().emplace<process_message>().message("完成数据数据创建");

  g_reg()->ctx().emplace<process_message>().message("检查数据库存在性");
  p_i->create_db(*in_connect);
  g_reg()->ctx().emplace<process_message>().message("开始插入数据库实体");
  p_i->insert_db_entity(*in_connect);
  g_reg()->ctx().emplace<process_message>().message("组件插入...");
  p_i->insert_db_com(*in_connect);

  g_reg()->ctx().emplace<process_message>().message("回调设置id");
  p_i->set_database_id();
  g_reg()->ctx().emplace<process_message>().message("完成");
}

}  // namespace doodle::database_n
