#pragma once
#include <memory>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include "presentation/middleware/AuthMiddleware.hpp"
#include "application/use_cases/exercises/SearchExercisesUseCase.hpp"
#include "application/use_cases/exercises/GetExerciseUseCase.hpp"
#include "application/use_cases/exercises/CreateExerciseUseCase.hpp"

namespace presentation {

class ExerciseController {
public:
    static void search(const drogon::HttpRequestPtr& req, const std::shared_ptr<RespCb>& cb,
        std::shared_ptr<application::SearchExercisesUseCase> useCase);

    static void getOne(const drogon::HttpRequestPtr& req, const std::shared_ptr<RespCb>& cb,
        int64_t exerciseId, std::shared_ptr<application::GetExerciseUseCase> useCase);

    static void create(const drogon::HttpRequestPtr& req, const std::shared_ptr<RespCb>& cb,
        int64_t userId, std::shared_ptr<application::CreateExerciseUseCase> useCase);
};

} // namespace presentation
