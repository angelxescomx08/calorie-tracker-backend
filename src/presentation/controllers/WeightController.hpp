#pragma once
#include <memory>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include "presentation/middleware/AuthMiddleware.hpp"
#include "application/use_cases/weight/ListWeightLogsUseCase.hpp"
#include "application/use_cases/weight/CreateWeightLogUseCase.hpp"
#include "application/use_cases/weight/UpdateWeightLogUseCase.hpp"
#include "application/use_cases/weight/DeleteWeightLogUseCase.hpp"

namespace presentation {

class WeightController {
public:
    static void list(const drogon::HttpRequestPtr& req, const std::shared_ptr<RespCb>& cb,
        int64_t userId, std::shared_ptr<application::ListWeightLogsUseCase> useCase);

    static void create(const drogon::HttpRequestPtr& req, const std::shared_ptr<RespCb>& cb,
        int64_t userId, std::shared_ptr<application::CreateWeightLogUseCase> useCase);

    static void update(const drogon::HttpRequestPtr& req, const std::shared_ptr<RespCb>& cb,
        int64_t logId, int64_t userId, std::shared_ptr<application::UpdateWeightLogUseCase> useCase);

    static void remove(const drogon::HttpRequestPtr& req, const std::shared_ptr<RespCb>& cb,
        int64_t logId, int64_t userId, std::shared_ptr<application::DeleteWeightLogUseCase> useCase);
};

} // namespace presentation
