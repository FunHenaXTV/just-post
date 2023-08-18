#include "create_user.hpp"

#include <fmt/format.h>

#include <userver/clients/dns/component.hpp>
#include <userver/components/component.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>
#include <userver/crypto/hash.hpp>


namespace pg_service_template {

namespace {

class CreateUser final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-create-user";

  CreateUser(const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context)
      : HttpHandlerBase(config, component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()) {}

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const override {
    // const auto& name = request.GetArg("name");

    // auto user_type = UserType::kFirstTime;
    // if (!name.empty()) {
    //   auto result = pg_cluster_->Execute(
    //       userver::storages::postgres::ClusterHostType::kMaster,
    //       "INSERT INTO hello_schema.users(name, count) VALUES($1, 1) "
    //       "ON CONFLICT (name) "
    //       "DO UPDATE SET count = users.count + 1 "
    //       "RETURNING users.count",
    //       name);

    //   if (result.AsSingleRow<int>() > 1) {
    //     user_type = UserType::kKnown;
    //   }
    // }

    return userver::crypto::hash::Sha512(request.RequestBody());
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendCreateUser(userver::components::ComponentList& component_list) {
  component_list.Append<CreateUser>();
}

}  // namespace pg_service_template
