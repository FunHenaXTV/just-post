#pragma once

#include <string>
#include <string_view>

#include <userver/components/component_list.hpp>

namespace just_post {

void AppendChangePasswd(userver::components::ComponentList& component_list);
bool IsValidPasswd(const std::string& s);

}  // namespace just_post