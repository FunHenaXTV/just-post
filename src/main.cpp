#include <userver/clients/dns/component.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>

#include "add-user_data.hpp"
#include "create_user.hpp"
#include "echo.hpp"
#include "get_id.hpp"
#include "hello.hpp"
#include "add-user_data.hpp"
#include "change-passwd.hpp"
#include "create_post.hpp"

int main(int argc, char* argv[]) {
  auto component_list =
      userver::components::MinimalServerComponentList()
          .Append<userver::server::handlers::Ping>()
          .Append<userver::components::TestsuiteSupport>()
          .Append<userver::components::HttpClient>()
          .Append<userver::server::handlers::TestsControl>()
          .Append<userver::components::Postgres>("postgres-db-1")
          .Append<userver::clients::dns::Component>();

  just_post::AppendCreateUser(component_list);
  just_post::AppendGetId(component_list);
  just_post::AppendHello(component_list);
  just_post::AppendEcho(component_list);
  just_post::AppendAddUserData(component_list);
  just_post::AppendChangePasswd(component_list);
  just_post::AppendCreatePost(component_list);

  return userver::utils::DaemonMain(argc, argv, component_list);
}
