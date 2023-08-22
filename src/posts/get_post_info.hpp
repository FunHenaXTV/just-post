#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <userver/components/component_list.hpp>

namespace just_post {
struct MyRowType {
  std::string us_email;
  std::string post_body;
  std::string date_of_post;
};

void AppendGetPostInfo(userver::components::ComponentList& component_list);
}  // namespace just_post