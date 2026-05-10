#pragma once
#include <memory>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include "presentation/middleware/AuthMiddleware.hpp"
#include "application/use_cases/exercise_logs/ListExerciseLogsUseCase.hpp"
#include "application/use_cases/exercise_logs/CreateExerciseLogUseCase.hpp"
#include "application/use_cases/exercise_logs/UpdateExerciseLogUseCase.hpp"
#include "application/use_cases/exercise_logs/DeleteExerciseLogUseCase.hpp"

namespace presentation {

class ExerciseLogController {
public:
    static void list(const drogon::HttpRequestPtr& req, const std::shared_ptr<RespCb>& cb,
        int64_t userId, std::shared_ptr<application::ListExerciseLogsUseCase> useCase);

    static void create(const drogon::HttpRequestPtr& req, const std::shared_ptr<RespCb>& cb,
        int64_t userId, std::shared_ptr<application::CreateExerciseLogUseCase> useCase);

    static void update(const drogon::HttpRequestPtr& req, const std::shared_ptr<RespCb>& cb,
        int64_t logId, int64_t userId, std::shared_ptr<application::UpdateExerciseLogUseCase> useCase);

    static void remove(const drogon::HttpRequestPtr& req, const std::shared_ptr<RespCb>& cb,
        int64_t logId, int64_t userId, std::shared_ptr<application::DeleteExerciseLogUseCase> useCase);
};

} // namespace presentation
