#include "get_last_n_post_id.hpp"

#include <userver/clients/dns/component.hpp>
#include <userver/components/component.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/storages/postgres/io/json_types.hpp>


namespace just_post {

namespace {

class GetLastNPostId final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-get-last-n-post_id";

  GetLastNPostId(const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context)
      : HttpHandlerBase(config, component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()) {}

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const override {
    const auto& cnt = request.GetArg("cnt");
    int n = strtol(cnt.c_str(), NULL, 10);

    //make vector of posts id from postgresql

    //if (cnt.size()) {
      //auto result = pg_cluster_->Execute(
          //userver::storages::postgres::ClusterHostType::kMaster,
        //  "SELECT post_id "
          //"FROM just_post_schema.posts "
        //  "ORDER BY date_of_post DESC "
      //    "LIMIT $1",
     //     n);

   //   if (result.Size()) {
  //      auto posts_id = result.AsContainer<std::vector<int>>();
        //std::cout << *(posts_id.begin()) << *(posts_id.end() - 1) << std::endl;
    //  }
   // }

   //trying return json string
   
       if (cnt.size()) {
      auto result = pg_cluster_->Execute(
          userver::storages::postgres::ClusterHostType::kMaster,
          "SELECT JSON_AGG(post_id) "
          "FROM just_post_schema.posts "
          "ORDER BY date_of_post DESC "
          "LIMIT $1",
          n);

      if (result.Size()) {
        return result.AsSingleRow<std::string>();
      }
    }

    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{
            "Too few params\n"});
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendGetLastNPostId(userver::components::ComponentList& component_list) {
  component_list.Append<GetLastNPostId>();
}

}  // namespace just_post
