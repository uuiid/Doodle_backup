//
// Created by TD on 2022/4/29.
//

#pragma once

#include <nlohmann/json.hpp>

#include <json_rpc/core/parser_rpc.h>

namespace boost::asio {
class io_context;
}

namespace doodle::json_rpc {

class rpc_client {
  class impl;
  std::unique_ptr<impl> ptr;

 public:
  explicit rpc_client(boost::asio::io_context& in_context,
                      const std::string& in_host,
                      std::uint16_t in_post);
  ~rpc_client();

 protected:
  std::string call_server(const std::string& in_string, bool is_notice);

  template <bool is_notice_type, typename Result_Type,
            typename... Args>
  auto call_fun(const std::string& in_name, Args... args) {
    nlohmann::json l_json{};

    rpc_request l_rpc_request{};
    l_rpc_request.method_   = in_name;
    l_rpc_request.is_notice = is_notice_type;

    if constexpr (sizeof...(args) > 0) {
      l_rpc_request.params_ = std::make_tuple(std::forward<Args>(args)...);
    }
    l_json = l_rpc_request;
    std::string l_json_str{};
    if constexpr (is_notice_type) {
      call_server(l_json.dump(), is_notice_type);
      return;
    } else {
      l_json_str = call_server(l_json.dump(), is_notice_type);
    }

    nlohmann::json l_r = nlohmann::json::parse(l_json_str);
    auto l_rpc_r       = l_r.template get<rpc_reply>();
    if (l_rpc_r.result.index() != rpc_reply::err_index) {
      if constexpr (std::is_same_v<void, Result_Type>)
        return;
      else
        return std::get<nlohmann::json>(l_rpc_r.result).template get<Result_Type>();
    } else {
      auto l_err_ = std::get<rpc_error>(l_rpc_r.result);
      l_err_.to_throw();
    }
    if constexpr (!std::is_same_v<void, Result_Type>)
      return Result_Type{};
  }
  void close();
};
}  // namespace doodle::json_rpc
