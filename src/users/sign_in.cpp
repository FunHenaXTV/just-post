#include "sign_in.hpp"

#include "../tools/verify_parameter.hpp"

#include <userver/clients/dns/component.hpp>
#include <userver/components/component.hpp>
#include <userver/crypto/hash.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>

namespace just_post {

namespace {

class SignIn final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-sign-in";

  SignIn(const userver::components::ComponentConfig& config,
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
    if (!tools::IsValidEmail(email) || !tools::IsValidPasswd(passwd)) {
      throw userver::server::handlers::ClientError(
          userver::server::handlers::ExternalBody{"Incorrect Parameters\n"});
    }

    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT passwd "
        "FROM just_post_schema.users "
        "WHERE email=$1",
        email);

    if (!result.Size()) {
      throw userver::server::handlers::ClientError(
          userver::server::handlers::ExternalBody{
              fmt::format("User with '{}' email does not exist\n", email)});
    }

    auto old_passwd = result.AsSingleRow<std::string>();
    auto passwd_hash = userver::crypto::hash::Sha512(passwd);
    if (passwd_hash != old_passwd) {
      throw userver::server::handlers::ClientError(
          userver::server::handlers::ExternalBody{"Incorrect password\n"});
    }

    return "1";
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendSignIn(userver::components::ComponentList& component_list) {
  component_list.Append<SignIn>();
}

}  // namespace just_post
