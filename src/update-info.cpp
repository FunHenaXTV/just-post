#include "hello.hpp"

#include <fmt/format.h>

#include <userver/clients/dns/component.hpp>
#include <userver/components/component.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>

namespace pg_service_template {

namespace {

class UpdateInfo final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-user_data-update_info";

  UpdateInfo(const userver::components::ComponentConfig& config,
             const userver::components::ComponentContext& component_context)
      : HttpHandlerBase(config, component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()) {}

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const override {
    const auto& user_id = request.GetArg("id");
    const auto& name = request.GetArg("name");
    const auto& surname = request.GetArg("surname");
    const auto& gender = request.GetArg("gender");
    const auto& city = request.GetArg("city");
    const auto& date_of_birth = request.GetArg("date_of_birth");
    const auto& status = request.GetArg("status");

    if (!user_id.empty() && !name.empty() && !surname.empty() &&
        !gender.empty() && !city.empty() && !date_of_birth.empty() &&
        !status.empty()) {
      auto result = pg_cluster_->Execute(
          userver::storages::postgres::ClusterHostType::kMaster,
          "INSERT INTO hello_schema.user_data(user_id, name, surname, gender, "
          "city, date_of_birth, status) "
          "VALUES($1, $2, $3, $4, $5, $6, $7) "
          "ON CONFLICT (user_id) "
          "DO NOTHING",
          //   "DO UPDATE SET count = users.count + 1 "
          //   "RETURNING users.count",
          user_id, name, surname, gender, city, date_of_birth, status);

      //   if (result.AsSingleRow<int>() > 1) {
      //     user_type = UserType::kKnown;
      //   }
    } else {
      return "0";
    }

    return "1";
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendUpdateInfo(userver::components::ComponentList& component_list) {
  component_list.Append<UpdateInfo>();
}

}  // namespace pg_service_template
