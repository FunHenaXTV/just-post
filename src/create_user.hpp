#pragma once

#include <string>
#include <string_view>
#include <vector>

#include <userver/components/component_list.hpp>

namespace pg_service_template {

void AppendCreateUser(userver::components::ComponentList& component_list);

}  // namespace pg_service_template