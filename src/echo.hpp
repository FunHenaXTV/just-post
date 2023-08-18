#pragma once

#include <string>
#include <string_view>

#include <userver/components/component_list.hpp>

namespace pg_service_template {

void AppendEcho(userver::components::ComponentList& component_list);

}  // namespace pg_service_template
