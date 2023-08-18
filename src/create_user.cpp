#include "create_user.hpp"

#include <fmt/format.h>

#include <userver/clients/dns/component.hpp>
#include <userver/components/component.hpp>
#include <userver/crypto/hash.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>

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
    const auto& name = request.GetArg("name");
    const auto& email = request.GetArg("email");
    const auto& password = request.GetArg("password");
    if (!name.empty() && !email.empty() && !password.empty()) {
      try {
        auto result = pg_cluster_->Execute(
            userver::storages::postgres::ClusterHostType::kMaster,
            "INSERT INTO hello_schema.user(name, email, password) "
            "VALUES ($1, $2, $3)",
            name, email, userver::crypto::hash::Sha512(password));
      } catch (userver::storages::postgres::UniqueViolation&) {
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{
                "User with this email already exists"});
      }

    } else {
      throw userver::server::handlers::ClientError(
          userver::server::handlers::ExternalBody{
              "Too few arguments to create user"});
    }

    return "1";
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendCreateUser(userver::components::ComponentList& component_list) {
  component_list.Append<CreateUser>();
}

}  // namespace pg_service_template
