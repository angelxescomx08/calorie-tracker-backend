#include "presentation/controllers/AuthController.hpp"
#include "presentation/Serializers.hpp"

namespace presentation {

void AuthController::registerUser(
    const drogon::HttpRequestPtr&              req,
    const std::shared_ptr<RespCb>&             cb,
    std::shared_ptr<application::RegisterUseCase> useCase)
{
    auto body = req->jsonObject();
    if (!body) { (*cb)(makeBadRequest("Request body must be JSON")); return; }

    std::string name     = (*body).get("name", "").asString();
    std::string email    = (*body).get("email", "").asString();
    std::string password = (*body).get("password", "").asString();

    if (name.empty() || email.empty() || password.size() < 8) {
        (*cb)(makeBadRequest("name, email and password (min 8 chars) are required"));
        return;
    }

    useCase->execute(name, email, password,
        [cb](domain::User user, domain::Session session) {
            Json::Value j;
            j["user"]       = serializeUser(user);
            j["token"]      = session.token;
            j["expires_at"] = session.expires_at;
            (*cb)(makeJsonResponse(j, drogon::k201Created));
        },
        [cb](std::string err) {
            (*cb)(makeBadRequest(err));
        });
}

void AuthController::login(
    const drogon::HttpRequestPtr&          req,
    const std::shared_ptr<RespCb>&         cb,
    std::shared_ptr<application::LoginUseCase> useCase)
{
    auto body = req->jsonObject();
    if (!body) { (*cb)(makeBadRequest("Request body must be JSON")); return; }

    std::string email    = (*body).get("email", "").asString();
    std::string password = (*body).get("password", "").asString();

    if (email.empty() || password.empty()) {
        (*cb)(makeBadRequest("email and password are required"));
        return;
    }

    useCase->execute(email, password,
        [cb](domain::User user, domain::Session session) {
            Json::Value j;
            j["user"]       = serializeUser(user);
            j["token"]      = session.token;
            j["expires_at"] = session.expires_at;
            (*cb)(makeJsonResponse(j));
        },
        [cb](std::string err) {
            (*cb)(makeError(drogon::k401Unauthorized, err));
        });
}

void AuthController::logout(
    const drogon::HttpRequestPtr&,
    const std::shared_ptr<RespCb>&         cb,
    const std::string&                     token,
    std::shared_ptr<application::LogoutUseCase> useCase)
{
    useCase->execute(token,
        [cb]()             { (*cb)(makeNoContent()); },
        [cb](std::string)  { (*cb)(makeNoContent()); }); // always 204
}

void AuthController::me(
    const drogon::HttpRequestPtr&,
    const std::shared_ptr<RespCb>&         cb,
    int64_t                                userId,
    std::shared_ptr<application::GetCurrentUserUseCase> useCase)
{
    useCase->execute(userId,
        [cb](domain::User user) { (*cb)(makeJsonResponse(serializeUser(user))); },
        [cb](std::string err)   { (*cb)(makeServerError(err)); });
}

} // namespace presentation
