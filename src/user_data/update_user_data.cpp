#include "update_user_data.hpp"
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

class UpdateUserData final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-user_data-update";

  UpdateUserData(const userver::components::ComponentConfig& config,
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

    int user_id_int = strtol(user_id.c_str(), NULL, 10);

    if (user_id.empty() || !tools::IsValidId(user_id_int)) {
      throw userver::server::handlers::ClientError(
          userver::server::handlers::ExternalBody{"Incorrect parameters\n"});
    }

    auto old_result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT user_name, surname, "
        "age, gender, "
        "city, date_of_birth, user_status "
        "FROM just_post_schema.user_data "
        "WHERE user_id=$1",
        user_id_int);

    auto params =
        old_result.AsSingleRow<User>(userver::storages::postgres::kRowTag);

    const auto& user_name = request.GetArg("user_name");
    const auto& surname = request.GetArg("surname");
    const auto& age = request.GetArg("age");
    const auto& gender = request.GetArg("gender");
    const auto& city = request.GetArg("city");
    const auto& date_of_birth_str = request.GetArg("date_of_birth");
    const auto& user_status = request.GetArg("user_status");

    if (!user_name.empty()) {
      params.name = user_name;
    }

    if (!surname.empty()) {
      params.surname = surname;
    }

    if (!age.empty()) {
      params.age = strtol(age.c_str(), NULL, 10);
    }

    if (tools::IsValidGender(gender)) {
      params.gender = gender;
    }

    if (!city.empty()) {
      params.city = city;
    }

    if (!date_of_birth_str.empty()) {
      int day = 0, month = 0, year = 0;
      sscanf(date_of_birth_str.c_str(), "%d-%d-%d", &year, &month, &day);
      userver::storages::postgres::Date date_of_birth(year, month, day);
      params.date_of_birth = date_of_birth;
    }

    if (!user_status.empty()) {
      params.status = user_status;
    }

    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "UPDATE just_post_schema.user_data "
        "SET user_name = $2, surname = $3, "
        "age = $4, gender = $5, "
        "city = $6, date_of_birth = $7, user_status =$8 "
        "WHERE user_id=$1 ",
        user_id_int, params.name, params.surname, params.age, params.gender,
        params.city, params.date_of_birth, params.status);

    return "ok\n";
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendUpdateUserData(userver::components::ComponentList& component_list) {
  component_list.Append<UpdateUserData>();
}

}  // namespace just_post
