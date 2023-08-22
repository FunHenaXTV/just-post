#pragma once

#include <string>

#include <userver/components/component_list.hpp>
#include "../tools/verify_parameter.hpp"

namespace just_post {

void AppendChangePasswd(userver::components::ComponentList& component_list);

}  // namespace just_post