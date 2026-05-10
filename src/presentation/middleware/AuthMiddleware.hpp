#pragma once
#include <functional>
#include <memory>
#include <string>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>

namespace presentation {

using RespCb = std::function<void(const drogon::HttpResponsePtr&)>;

class AuthMiddleware {
public:
    // Extracts "Authorization: Bearer <token>", validates it against the DB.
    // On success calls onAuth(userId, token); on failure calls (*respCb)(401).
    static void authenticate(
        const drogon::HttpRequestPtr&      req,
        const std::shared_ptr<RespCb>&     respCb,
        std::function<void(int64_t, const std::string&)> onAuth);
};

} // namespace presentation
