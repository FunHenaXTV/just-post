#pragma once

#include <string>

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

void AppendUpdateUserData(userver::components::ComponentList& component_list);

}  // namespace just_post