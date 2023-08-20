#pragma once

#include <string>
#include <string_view>

#include <userver/components/component_list.hpp>

namespace just_post {

void AppendCreatePost(userver::components::ComponentList& component_list);

}  // namespace just_post