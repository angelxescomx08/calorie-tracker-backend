#pragma once
#include <memory>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include "presentation/middleware/AuthMiddleware.hpp"
#include "application/use_cases/goals/GetActiveGoalUseCase.hpp"
#include "application/use_cases/goals/CreateGoalUseCase.hpp"
#include "application/use_cases/goals/UpdateGoalUseCase.hpp"
#include "application/use_cases/goals/DeactivateGoalUseCase.hpp"

namespace presentation {

class GoalController {
public:
    static void getActive(const drogon::HttpRequestPtr& req, const std::shared_ptr<RespCb>& cb,
        int64_t userId, std::shared_ptr<application::GetActiveGoalUseCase> useCase);

    static void create(const drogon::HttpRequestPtr& req, const std::shared_ptr<RespCb>& cb,
        int64_t userId, std::shared_ptr<application::CreateGoalUseCase> useCase);

    static void update(const drogon::HttpRequestPtr& req, const std::shared_ptr<RespCb>& cb,
        int64_t goalId, int64_t userId, std::shared_ptr<application::UpdateGoalUseCase> useCase);

    static void deactivate(const drogon::HttpRequestPtr& req, const std::shared_ptr<RespCb>& cb,
        int64_t goalId, int64_t userId, std::shared_ptr<application::DeactivateGoalUseCase> useCase);
};

} // namespace presentation
