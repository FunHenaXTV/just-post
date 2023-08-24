#include "get_comment_info.hpp"
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

class GetCommentInfo final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-get-comment-info";

  GetCommentInfo(const userver::components::ComponentConfig& config,
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

    if (!comment_id.size()) {
      throw userver::server::handlers::ClientError(
          userver::server::handlers::ExternalBody{"Incorrect parameters\n"});
    }

    int int_comment_id = strtol(comment_id.c_str(), NULL, 10);

    if (!tools::IsValidId(int_comment_id)) {
      throw userver::server::handlers::ClientError(
          userver::server::handlers::ExternalBody{"Incorrect parameters\n"});
    }

    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT just_post_schema.users.email, comment_body, "
        "to_char(date_of_comment, 'YYYY-MM-DD HH24:MI:SS+03:00')"
        "FROM just_post_schema.comments "
        "JOIN just_post_schema.users USING(user_id)"
        "WHERE comment_id = $1 ",
        int_comment_id);

    //нужен ли post_id ????

    if (!result.Size()) {
      throw userver::server::handlers::ClientError(
          userver::server::handlers::ExternalBody{
              "This comment does not exist\n"});
    }

    auto iteration = result.AsSetOf<just_post::RowTypeComments>(
        userver::storages::postgres::kRowTag);

    auto row = *(iteration.begin());
    static_assert(std::is_same_v<decltype(row), just_post::RowTypeComments>,
                  "Iterate over aggregate classes");

    std::string json_comment_info = "{";
    json_comment_info += "\"email\":";
    json_comment_info += "\"" + row.us_email + "\"";
    json_comment_info += ", \"comment_body\":";
    json_comment_info += "\"" + row.comment_body + "\"";
    json_comment_info += ", \"date_of_comment\":";
    json_comment_info +=
        "\"" + row.date_of_comment.substr(0, 10) + "T" +
        row.date_of_comment.substr(11, row.date_of_comment.size()) + "\"";
    json_comment_info += "}";

    return json_comment_info;
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendGetCommentInfo(userver::components::ComponentList& component_list) {
  component_list.Append<GetCommentInfo>();
}

}  // namespace just_post