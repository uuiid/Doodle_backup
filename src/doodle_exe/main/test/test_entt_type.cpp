//
// Created by TD on 2022/5/30.
//
#include <catch2/catch_all.hpp>

#include <doodle_lib/doodle_lib_fwd.h>
#include <doodle_lib/app/app.h>

TEST_CASE("entt type") {
  std::cout << entt::type_id<std::vector<std::string>>().name() << std::endl;
  std::cout << entt::type_id<std::vector<std::string>>().hash() << std::endl;
  std::cout << entt::type_id<std::vector<std::string>>().index() << std::endl;
}
