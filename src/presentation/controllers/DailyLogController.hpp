#pragma once
#include <memory>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include "presentation/middleware/AuthMiddleware.hpp"
#include "application/use_cases/daily/GetDailyLogUseCase.hpp"
#include "application/use_cases/daily/UpdateDailyLogUseCase.hpp"
#include "application/use_cases/daily/AddMealEntryUseCase.hpp"
#include "application/use_cases/daily/UpdateMealEntryUseCase.hpp"
#include "application/use_cases/daily/DeleteMealEntryUseCase.hpp"

namespace presentation {

class DailyLogController {
public:
    static void getByDate(const drogon::HttpRequestPtr& req, const std::shared_ptr<RespCb>& cb,
        int64_t userId, std::shared_ptr<application::GetDailyLogUseCase> useCase);

    static void update(const drogon::HttpRequestPtr& req, const std::shared_ptr<RespCb>& cb,
        int64_t logId, int64_t userId, std::shared_ptr<application::UpdateDailyLogUseCase> useCase);

    static void addMeal(const drogon::HttpRequestPtr& req, const std::shared_ptr<RespCb>& cb,
        int64_t userId, std::shared_ptr<application::AddMealEntryUseCase> useCase);

    static void updateMeal(const drogon::HttpRequestPtr& req, const std::shared_ptr<RespCb>& cb,
        int64_t entryId, int64_t logId, std::shared_ptr<application::UpdateMealEntryUseCase> useCase);

    static void deleteMeal(const drogon::HttpRequestPtr& req, const std::shared_ptr<RespCb>& cb,
        int64_t entryId, int64_t logId, std::shared_ptr<application::DeleteMealEntryUseCase> useCase);
};

} // namespace presentation
