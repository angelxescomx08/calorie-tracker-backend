#pragma once
#include <memory>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include "presentation/middleware/AuthMiddleware.hpp"
#include "application/use_cases/foods/SearchFoodsUseCase.hpp"
#include "application/use_cases/foods/GetFoodUseCase.hpp"
#include "application/use_cases/foods/CreateFoodUseCase.hpp"
#include "application/use_cases/foods/UpdateFoodUseCase.hpp"
#include "application/use_cases/foods/DeleteFoodUseCase.hpp"

namespace presentation {

class FoodController {
public:
    static void search(const drogon::HttpRequestPtr& req, const std::shared_ptr<RespCb>& cb,
        std::shared_ptr<application::SearchFoodsUseCase> useCase);

    static void getOne(const drogon::HttpRequestPtr& req, const std::shared_ptr<RespCb>& cb,
        int64_t foodId, std::shared_ptr<application::GetFoodUseCase> useCase);

    static void create(const drogon::HttpRequestPtr& req, const std::shared_ptr<RespCb>& cb,
        int64_t userId, std::shared_ptr<application::CreateFoodUseCase> useCase);

    static void update(const drogon::HttpRequestPtr& req, const std::shared_ptr<RespCb>& cb,
        int64_t foodId, std::shared_ptr<application::UpdateFoodUseCase> useCase);

    static void remove(const drogon::HttpRequestPtr& req, const std::shared_ptr<RespCb>& cb,
        int64_t foodId, int64_t userId, std::shared_ptr<application::DeleteFoodUseCase> useCase);
};

} // namespace presentation
