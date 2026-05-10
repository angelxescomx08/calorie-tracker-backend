#include "presentation/controllers/FoodController.hpp"
#include "presentation/Serializers.hpp"

namespace presentation {

void FoodController::search(
    const drogon::HttpRequestPtr& req,
    const std::shared_ptr<RespCb>& cb,
    std::shared_ptr<application::SearchFoodsUseCase> useCase)
{
    std::string query  = req->getParameter("q");
    int limit  = 20;
    int offset = 0;
    auto ls = req->getParameter("limit");
    auto os = req->getParameter("offset");
    if (!ls.empty()) limit  = std::stoi(ls);
    if (!os.empty()) offset = std::stoi(os);

    useCase->execute(query, limit, offset,
        [cb, limit, offset](std::vector<domain::Food> foods, int total) {
            Json::Value arr(Json::arrayValue);
            for (const auto& f : foods) arr.append(serializeFood(f));
            Json::Value j;
            j["items"]  = arr;
            j["total"]  = total;
            j["limit"]  = limit;
            j["offset"] = offset;
            (*cb)(makeJsonResponse(j));
        },
        [cb](std::string err) { (*cb)(makeServerError(err)); });
}

void FoodController::getOne(
    const drogon::HttpRequestPtr&,
    const std::shared_ptr<RespCb>& cb,
    int64_t foodId,
    std::shared_ptr<application::GetFoodUseCase> useCase)
{
    useCase->execute(foodId,
        [cb](std::optional<domain::Food> f) {
            if (!f) { (*cb)(makeNotFound("Food not found")); return; }
            (*cb)(makeJsonResponse(serializeFood(*f)));
        },
        [cb](std::string err) { (*cb)(makeServerError(err)); });
}

void FoodController::create(
    const drogon::HttpRequestPtr& req,
    const std::shared_ptr<RespCb>& cb,
    int64_t userId,
    std::shared_ptr<application::CreateFoodUseCase> useCase)
{
    auto body = req->jsonObject();
    if (!body) { (*cb)(makeBadRequest("Request body must be JSON")); return; }

    domain::Food food{};
    food.name               = (*body).get("name", "").asString();
    food.brand              = (*body).get("brand", "").asString();
    food.barcode            = (*body).get("barcode", "").asString();
    food.serving_size_g     = (*body).get("serving_size_g", 100.0).asDouble();
    food.calories           = (*body).get("calories", 0.0).asDouble();
    food.protein_g          = (*body).get("protein_g", 0.0).asDouble();
    food.carbs_g            = (*body).get("carbs_g", 0.0).asDouble();
    food.fat_g              = (*body).get("fat_g", 0.0).asDouble();
    food.fiber_g            = (*body).get("fiber_g", 0.0).asDouble();
    food.sugar_g            = (*body).get("sugar_g", 0.0).asDouble();
    food.sodium_mg          = (*body).get("sodium_mg", 0.0).asDouble();
    food.created_by_user_id = userId;
    food.is_verified        = false;

    if (food.name.empty()) {
        (*cb)(makeBadRequest("name is required"));
        return;
    }

    useCase->execute(std::move(food),
        [cb](domain::Food f) { (*cb)(makeJsonResponse(serializeFood(f), drogon::k201Created)); },
        [cb](std::string err) { (*cb)(makeServerError(err)); });
}

void FoodController::update(
    const drogon::HttpRequestPtr& req,
    const std::shared_ptr<RespCb>& cb,
    int64_t foodId,
    std::shared_ptr<application::UpdateFoodUseCase> useCase)
{
    auto body = req->jsonObject();
    if (!body) { (*cb)(makeBadRequest("Request body must be JSON")); return; }

    domain::Food food{};
    food.id             = foodId;
    food.name           = (*body).get("name", "").asString();
    food.brand          = (*body).get("brand", "").asString();
    food.barcode        = (*body).get("barcode", "").asString();
    food.serving_size_g = (*body).get("serving_size_g", 100.0).asDouble();
    food.calories       = (*body).get("calories", 0.0).asDouble();
    food.protein_g      = (*body).get("protein_g", 0.0).asDouble();
    food.carbs_g        = (*body).get("carbs_g", 0.0).asDouble();
    food.fat_g          = (*body).get("fat_g", 0.0).asDouble();
    food.fiber_g        = (*body).get("fiber_g", 0.0).asDouble();
    food.sugar_g        = (*body).get("sugar_g", 0.0).asDouble();
    food.sodium_mg      = (*body).get("sodium_mg", 0.0).asDouble();

    useCase->execute(std::move(food),
        [cb](domain::Food f) { (*cb)(makeJsonResponse(serializeFood(f))); },
        [cb](std::string err) { (*cb)(makeServerError(err)); });
}

void FoodController::remove(
    const drogon::HttpRequestPtr&,
    const std::shared_ptr<RespCb>& cb,
    int64_t foodId, int64_t userId,
    std::shared_ptr<application::DeleteFoodUseCase> useCase)
{
    useCase->execute(foodId, userId,
        [cb]()            { (*cb)(makeNoContent()); },
        [cb](std::string err) { (*cb)(makeServerError(err)); });
}

} // namespace presentation
