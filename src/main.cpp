#include <userver/clients/dns/component.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>

#include "echo/echo.hpp"
#include "hello/hello.hpp"

#include "users/change_passwd.hpp"
#include "users/create_user.hpp"
#include "users/get_id.hpp"

#include "posts/get_last_n_post_id.hpp"
#include "posts/create_post.hpp"
#include "posts/delete_post.hpp"

#include "user_data/add_user_data.hpp"

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
  just_post::AppendGetLastNPostId(component_list);
  just_post::AppendDeletePost(component_list);

  return userver::utils::DaemonMain(argc, argv, component_list);
}
