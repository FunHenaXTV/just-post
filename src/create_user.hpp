#pragma once

#include <string>
#include <string_view>

#include <userver/components/component_list.hpp>

namespace just_post {

void AppendCreateUser(userver::components::ComponentList& component_list);
bool IsValidEmail(const std::string &s);
bool IsValidPasswd(const std::string &s);

}  // namespace just_post
