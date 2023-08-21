#include "change_passwd.hpp"

#include <fmt/format.h>

#include <userver/clients/dns/component.hpp>
#include <userver/components/component.hpp>
#include <userver/crypto/hash.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>

namespace just_post {
static constexpr int MIN_SIZE_OF_PSWD = 8;

namespace {

bool IsValidPasswd(const std::string& s);

class ChangePasswd final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-change-passwd";

  ChangePasswd(const userver::components::ComponentConfig& config,
             const userver::components::ComponentContext& component_context)
      : HttpHandlerBase(config, component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()) {}

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const override {
    const auto& user_id = request.GetArg("user_id");
    const auto& old_passwd = request.GetArg("old_passwd");
    const auto& new_passwd = request.GetArg("new_passwd");

    if (user_id.empty() || old_passwd.empty() || new_passwd.empty()) {
      throw userver::server::handlers::ClientError(
          userver::server::handlers::ExternalBody{
              "Too few arguments to change password\n"});
    }

    auto hash_old_passwd = userver::crypto::hash::Sha512(old_passwd);
    int user_id_int = strtol(user_id.c_str(), NULL, 10);

    auto bd_answer = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT passwd "
        "FROM just_post_schema.users "
        "WHERE user_id=$1",
        user_id_int);

    if (bd_answer.Size() == 1) {
      auto bd_passwd = bd_answer.AsSingleRow<std::string>();

      if (bd_passwd != hash_old_passwd) {
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{
                "Wrong password for this user\n"});
      }
    } else {
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{
                "This user havent registered yet\n"});
    }

    auto hash_new_passwd = userver::crypto::hash::Sha512(new_passwd);

    if (hash_new_passwd == hash_old_passwd) {
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{
                "New password matched with old password\n"});
    }

    if (!IsValidPasswd(new_passwd)) {
      throw userver::server::handlers::ClientError(
          userver::server::handlers::ExternalBody{
              "Password must contain at least 8 symbols\n"});
    }

    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "UPDATE just_post_schema.users "
        "SET passwd=$2 "
        "WHERE user_id=$1 ",
        user_id_int, hash_new_passwd);

    if (result.RowsAffected()) {
      request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);
      return "ok\n";
    }

    return "Server Error\n";
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

bool IsValidPasswd(const std::string& s) {
  return s.size() >= MIN_SIZE_OF_PSWD;
}

}  // namespace

void AppendChangePasswd(userver::components::ComponentList& component_list) {
  component_list.Append<ChangePasswd>();
}

}  // namespace just_post
