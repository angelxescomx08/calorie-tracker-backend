#pragma once
#include <memory>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include "presentation/middleware/AuthMiddleware.hpp"
#include "application/use_cases/auth/RegisterUseCase.hpp"
#include "application/use_cases/auth/LoginUseCase.hpp"
#include "application/use_cases/auth/LogoutUseCase.hpp"
#include "application/use_cases/auth/GetCurrentUserUseCase.hpp"

namespace presentation {

class AuthController {
public:
    static void registerUser(
        const drogon::HttpRequestPtr&          req,
        const std::shared_ptr<RespCb>&         cb,
        std::shared_ptr<application::RegisterUseCase> useCase);

    static void login(
        const drogon::HttpRequestPtr&          req,
        const std::shared_ptr<RespCb>&         cb,
        std::shared_ptr<application::LoginUseCase> useCase);

    static void logout(
        const drogon::HttpRequestPtr&          req,
        const std::shared_ptr<RespCb>&         cb,
        const std::string&                     token,
        std::shared_ptr<application::LogoutUseCase> useCase);

    static void me(
        const drogon::HttpRequestPtr&          req,
        const std::shared_ptr<RespCb>&         cb,
        int64_t                                userId,
        std::shared_ptr<application::GetCurrentUserUseCase> useCase);
};

} // namespace presentation
