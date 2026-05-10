#include "presentation/middleware/AuthMiddleware.hpp"
#include "presentation/Serializers.hpp"
#include "infrastructure/persistence/PostgresSessionRepository.hpp"
#include <drogon/drogon.h>

namespace presentation {

void AuthMiddleware::authenticate(
    const drogon::HttpRequestPtr&      req,
    const std::shared_ptr<RespCb>&     respCb,
    std::function<void(int64_t, const std::string&)> onAuth)
{
    const std::string& authHeader = req->getHeader("Authorization");
    if (authHeader.size() < 8 || authHeader.substr(0, 7) != "Bearer ") {
        (*respCb)(makeUnauthorized("Missing or invalid Authorization header"));
        return;
    }

    std::string token = authHeader.substr(7);
    if (token.empty()) {
        (*respCb)(makeUnauthorized("Empty token"));
        return;
    }

    auto repo = std::make_shared<infrastructure::PostgresSessionRepository>(
        drogon::app().getDbClient());

    repo->findByToken(token,
        [respCb, onAuth = std::move(onAuth)](std::optional<domain::Session> session) {
            if (!session) {
                (*respCb)(makeUnauthorized("Invalid or expired token"));
                return;
            }
            onAuth(session->user_id, session->token);
        },
        [respCb](std::string) {
            (*respCb)(makeServerError("Authentication check failed"));
        });
}

} // namespace presentation
