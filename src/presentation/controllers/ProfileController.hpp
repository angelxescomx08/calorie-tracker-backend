#pragma once
#include <memory>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include "presentation/middleware/AuthMiddleware.hpp"
#include "application/use_cases/profile/GetProfileUseCase.hpp"
#include "application/use_cases/profile/UpsertProfileUseCase.hpp"

namespace presentation {

class ProfileController {
public:
    static void getProfile(
        const drogon::HttpRequestPtr& req,
        const std::shared_ptr<RespCb>& cb,
        int64_t userId,
        std::shared_ptr<application::GetProfileUseCase> useCase);

    static void upsertProfile(
        const drogon::HttpRequestPtr& req,
        const std::shared_ptr<RespCb>& cb,
        int64_t userId,
        std::shared_ptr<application::UpsertProfileUseCase> useCase);
};

} // namespace presentation
