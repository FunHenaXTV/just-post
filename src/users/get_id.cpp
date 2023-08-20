#include "get_id.hpp"

#include <userver/clients/dns/component.hpp>
#include <userver/components/component.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>

namespace just_post {

namespace {

class GetId final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-get-id";

  GetId(const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context)
      : HttpHandlerBase(config, component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()) {}

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const override {
    const auto& email = request.GetArg("email");
    if (email.size()) {
      auto result = pg_cluster_->Execute(
          userver::storages::postgres::ClusterHostType::kMaster,
          "SELECT user_id "
          "FROM just_post_schema.users "
          "WHERE email=$1",
          email);
      if (result.Size()) {
        return std::to_string(result.AsSingleRow<int>()) + "\n";
      }
    }

    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{
            "Email is invalid or user does not exist\n"});
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendGetId(userver::components::ComponentList& component_list) {
  component_list.Append<GetId>();
}

}  // namespace just_post
