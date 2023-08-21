#include "create_user.hpp"
#include "../tools/verify_parameter.hpp"

#include <fmt/format.h>
#include <regex>

#include <userver/clients/dns/component.hpp>
#include <userver/components/component.hpp>
#include <userver/crypto/hash.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>

namespace just_post {
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
    const auto& email = request.GetArg("email");
    const auto& passwd = request.GetArg("passwd");
    if (!tools::IsValidEmail(email)) {
      throw userver::server::handlers::ClientError(
          userver::server::handlers::ExternalBody{"Email is invalid\n"});
    }

    if (!tools::IsValidPasswd(passwd)) {
      throw userver::server::handlers::ClientError(
          userver::server::handlers::ExternalBody{
              "Password must contain at least 8 symbols\n"});
    }

    auto hash_passwd = userver::crypto::hash::Sha512(passwd);
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO just_post_schema.users(email, passwd) "
        "VALUES ($1, $2) "
        "ON CONFLICT DO NOTHING",
        email, hash_passwd);
    if (result.RowsAffected()) {
      request.SetResponseStatus(
          userver::server::http::HttpStatus::kCreated);  // 201
      return "ok\n";
    }

    result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT passwd "
        "FROM just_post_schema.users "
        "WHERE email=$1",
        email);

    auto s = result.AsSingleRow<std::string>();
    if (s != hash_passwd) {
      throw userver::server::handlers::ClientError(
          userver::server::handlers::ExternalBody{
              "User with this email already exists\n"});
    }

    return "ok\n";
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendCreateUser(userver::components::ComponentList& component_list) {
  component_list.Append<CreateUser>();
}

}  // namespace just_post
