#pragma once

#include <string>
#include <string_view>

#include <userver/components/component_list.hpp>

namespace pg_service_template {

void AppendUpdateInfo(userver::components::ComponentList& component_list);

}  // namespace pg_service_template
