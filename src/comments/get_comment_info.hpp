#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <userver/components/component_list.hpp>

namespace just_post {
  struct RowTypeComments {
    std::string us_email;
    std::string comment_body;
    std::string date_of_comment;
  };

  void AppendGetCommentInfo(userver::components::ComponentList& component_list);
}  // namespace just_post