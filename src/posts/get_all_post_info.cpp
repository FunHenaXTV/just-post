#include "get_all_post_info.hpp"
#include "../tools/verify_parameter.hpp"

#include <userver/clients/dns/component.hpp>
#include <userver/components/component.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/io/date.hpp>
#include <userver/utils/assert.hpp>

namespace just_post {

namespace {

class GetAllPostInfo final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-get-all-post-info";

  GetAllPostInfo(const userver::components::ComponentConfig& config,
                 const userver::components::ComponentContext& component_context)
      : HttpHandlerBase(config, component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()) {}

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const override {
    const auto& post_id = request.GetArg("post_id");

    if (post_id.size()) {
      int int_post_id = strtol(post_id.c_str(), NULL, 10);

      if (int_post_id <= 0) {
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{"Incorrect params\n"});
      }

      auto result = pg_cluster_->Execute(
          userver::storages::postgres::ClusterHostType::kMaster,
          "SELECT just_post_schema.users.email, post_body, to_char(date_of_post, 'YYYY-MM-DD HH24:MI:SS+03:00')"
          "FROM just_post_schema.posts "
          "JOIN just_post_schema.users USING(user_id)"
          "WHERE post_id = $1 ",
          int_post_id);

        if (!result.Size()) {
            throw userver::server::handlers::ClientError(
                userver::server::handlers::ExternalBody{"This post doesnt exist\n"});
        }

        auto iteration = result.AsSetOf<just_post::MyRowType>(userver::storages::postgres::kRowTag);

        for (auto row : iteration) {
            static_assert(std::is_same_v<decltype(row), just_post::MyRowType>,
                "Iterate over aggregate classes");

            std::string json_post_info = "{";
            json_post_info += "\"email\":";
            json_post_info += "\"" + row.us_email + "\"";
            json_post_info += ", \"post_body\":";
            json_post_info += "\"" + row.post_body + "\"";
            json_post_info += ", \"date_of_post\":";
            json_post_info += "\"" + row.date_of_post.substr(0,10) + "T" + row.date_of_post.substr(11,row.date_of_post.size()) + "\"";
            json_post_info += "}";

            return json_post_info;
        }
    }

    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{"Incorrect parameters\n"});
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendGetAllPostInfo(userver::components::ComponentList& component_list) {
  component_list.Append<GetAllPostInfo>();
}

}  // namespace just_post