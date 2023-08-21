#pragma once

#include <string>
#include <string_view>

#include <userver/components/component_list.hpp>

namespace just_post {

enum { shift_for_month = 1, shift_for_year = 1900 };

void AppendCreatePost(userver::components::ComponentList& component_list);

}  // namespace just_post