#include "echo.hpp"

#include <fmt/format.h>

#include <userver/components/component.hpp>
#include <userver/crypto/hash.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/utils/assert.hpp>
namespace pg_service_template {

namespace {

class Echo final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-echo";

  using HttpHandlerBase::HttpHandlerBase;

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const override {
    return request.RequestBody() + "\n";
  }
};

}  // namespace

void AppendEcho(userver::components::ComponentList& component_list) {
  component_list.Append<Echo>();
}

}  // namespace pg_service_template