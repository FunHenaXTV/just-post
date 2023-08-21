#pragma once

#include <string>
#include <string_view>

#include <userver/components/component_list.hpp>
#include <userver/storages/postgres/io/date.hpp>

namespace just_post {

struct User {
  std::string name;
  std::string surname;
  int age;
  std::string gender;
  std::string city;
  userver::storages::postgres::Date date_of_birth;
  std::string status;
};

// struct Test{
//     std::string test1;
//     std::string test2;
//     int test3;
// };

void AppendUpdateUserData(userver::components::ComponentList& component_list);

}  // namespace just_post