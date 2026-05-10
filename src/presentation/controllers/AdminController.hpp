#pragma once
#include <memory>
#include <functional>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include "application/use_cases/SeedExercisesUseCase.hpp"

namespace presentation {

class AdminController {
public:
    explicit AdminController(
        std::shared_ptr<application::SeedExercisesUseCase> seedUseCase
    );

    void seedExercises(
        const drogon::HttpRequestPtr&                              req,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback
    );

private:
    std::shared_ptr<application::SeedExercisesUseCase> seedUseCase_;
};

} // namespace presentation
