#include "get_n_comments.hpp"
#include "../tools/verify_parameter.hpp"

#include <userver/clients/dns/component.hpp>
#include <userver/components/component.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/io/json_types.hpp>
#include <userver/utils/assert.hpp>

namespace just_post {

namespace {

class GetNComments final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-get-n-comments";

  GetNComments(const userver::components::ComponentConfig& config,
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
    const auto& cnt = request.GetArg("n");

    if (!cnt.size() || !post_id.size()) {
        throw userver::server::handlers::ClientError(
          userver::server::handlers::ExternalBody{"Incorrect parameters\n"});
    }

    int n = strtol(cnt.c_str(), NULL, 10);
    int int_post_id = strtol(post_id.c_str(), NULL, 10);

    if (n <= 0 || !tools::IsValidId(int_post_id)) {
      throw userver::server::handlers::ClientError(
          userver::server::handlers::ExternalBody{"Incorrect parameters\n"});
    }

    auto result = pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kMaster,
      "SELECT comment_id "
      "FROM just_post_schema.comments "
      "WHERE post_id = $2"
      "ORDER BY date_of_comment DESC "
      "LIMIT $1",
      n, int_post_id);

    if (result.Size()) {
      auto comments_id = result.AsContainer<std::vector<int>>();
      std::string json_comments_id= "[";

      for (auto i = comments_id.begin(); i < comments_id.end(); i++) {
        json_comments_id += std::to_string(*i);

        if (i != comments_id.end() - 1) {
          json_comments_id += ", ";
        }
      }

      json_comments_id += "]";

      return json_comments_id;
    } else {
      throw userver::server::handlers::ClientError(
          userver::server::handlers::ExternalBody{"This post does not exist or have not any comments\n"});
    }
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendGetNComments(userver::components::ComponentList& component_list) {
  component_list.Append<GetNComments>();
}

}  // namespace just_post
