#include "add_user_data.hpp"

#include <fmt/format.h>
#include <iostream>

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

class AddUserData final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-user_data-add";

  AddUserData(const userver::components::ComponentConfig& config,
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
    const auto& user_name = request.GetArg("user_name");
    const auto& surname = request.GetArg("surname");
    const auto& age = request.GetArg("age");
    const auto& gender = request.GetArg("gender");
    const auto& city = request.GetArg("city");
    const auto& date_of_birth_str = request.GetArg("date_of_birth");
    const auto& user_status = request.GetArg("user_status");

    // + 10 ифов

    if (user_id.empty() || user_name.empty() || surname.empty() ||
        gender.empty() || city.empty() || date_of_birth_str.empty() ||
        user_status.empty() || age.empty()) {
      throw userver::server::handlers::ClientError(
          userver::server::handlers::ExternalBody{"Incorrect parametrs\n"});
    }

    int user_id_int = strtol(user_id.c_str(), NULL, 10);
    int age_int = strtol(age.c_str(), NULL, 10);
    int day = 0, month = 0, year = 0;
    sscanf(date_of_birth_str.c_str(), "%d-%d-%d", &year, &month, &day);
    userver::storages::postgres::Date date_of_birth(year, month, day);

    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO just_post_schema.user_data(user_id, user_name, surname, "
        "age, gender, "
        "city, date_of_birth, user_status) "
        "VALUES($1, $2, $3, $4, $5, $6, $7, $8) "
        "ON CONFLICT (user_id) "
        "DO NOTHING",
        user_id_int, user_name, surname, age_int, gender, city, date_of_birth,
        user_status);

    if (result.RowsAffected()) {
      request.SetResponseStatus(
          userver::server::http::HttpStatus::kCreated);  // 201
    }

    // TODO: Conflict check

    return "ok\n";
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendAddUserData(userver::components::ComponentList& component_list) {
  component_list.Append<AddUserData>();
}

}  // namespace just_post
