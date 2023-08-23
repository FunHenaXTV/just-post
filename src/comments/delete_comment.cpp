#include "delete_comment.hpp"
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

class DeleteComment final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-delete-comment";

  DeleteComment(const userver::components::ComponentConfig& config,
             const userver::components::ComponentContext& component_context)
      : HttpHandlerBase(config, component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()) {}

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const override {
    const auto& comment_id = request.GetArg("comment_id");
    int comment_id_int = strtol(comment_id.c_str(), NULL, 10);

    if (comment_id.empty() || !tools::IsValidId(comment_id_int)) {
      throw userver::server::handlers::ClientError(
          userver::server::handlers::ExternalBody{"Incorrect parameters\n"});
    }

    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "DELETE FROM just_post_schema.comments "
        "WHERE comment_id=$1",
        comment_id_int);
    if (result.RowsAffected()) {
      return "ok\n";
    }

    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{
            "Comment with this id was not exist or already deleted\n"});
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendDeleteComment(userver::components::ComponentList& component_list) {
  component_list.Append<DeleteComment>();
}

}  // namespace just_post
