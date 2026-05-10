#include "presentation/controllers/DailyLogController.hpp"
#include "presentation/Serializers.hpp"

namespace presentation {

void DailyLogController::getByDate(
    const drogon::HttpRequestPtr& req,
    const std::shared_ptr<RespCb>& cb,
    int64_t userId,
    std::shared_ptr<application::GetDailyLogUseCase> useCase)
{
    std::string date = req->getParameter("date");
    if (date.empty()) {
        // Default to today.
        auto now = std::time(nullptr);
        std::tm tm{};
#ifdef _WIN32
        gmtime_s(&tm, &now);
#else
        gmtime_r(&now, &tm);
#endif
        char buf[11];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d", &tm);
        date = buf;
    }

    useCase->execute(userId, date,
        [cb](application::DailyLogWithEntries result) {
            Json::Value j = serializeDailyLog(result.log);
            Json::Value arr(Json::arrayValue);
            for (const auto& e : result.entries) arr.append(serializeMealEntry(e));
            j["meal_entries"] = arr;
            (*cb)(makeJsonResponse(j));
        },
        [cb](std::string err) { (*cb)(makeServerError(err)); });
}

void DailyLogController::update(
    const drogon::HttpRequestPtr& req,
    const std::shared_ptr<RespCb>& cb,
    int64_t logId, int64_t userId,
    std::shared_ptr<application::UpdateDailyLogUseCase> useCase)
{
    auto body = req->jsonObject();
    if (!body) { (*cb)(makeBadRequest("Request body must be JSON")); return; }

    domain::DailyLog log{};
    log.id       = logId;
    log.user_id  = userId;
    log.water_ml = (*body).get("water_ml", 0).asInt();
    log.notes    = (*body).get("notes", "").asString();

    useCase->execute(std::move(log),
        [cb](domain::DailyLog l) { (*cb)(makeJsonResponse(serializeDailyLog(l))); },
        [cb](std::string err)    { (*cb)(makeServerError(err)); });
}

void DailyLogController::addMeal(
    const drogon::HttpRequestPtr& req,
    const std::shared_ptr<RespCb>& cb,
    int64_t userId,
    std::shared_ptr<application::AddMealEntryUseCase> useCase)
{
    auto body = req->jsonObject();
    if (!body) { (*cb)(makeBadRequest("Request body must be JSON")); return; }

    std::string date      = (*body).get("date", "").asString();
    std::string mealTypeS = (*body).get("meal_type", "snack").asString();
    int64_t foodId        = static_cast<int64_t>((*body).get("food_id", 0).asInt64());
    double  quantityG     = (*body).get("quantity_g", 0.0).asDouble();

    domain::MealType mealType;
    if      (mealTypeS == "breakfast") mealType = domain::MealType::Breakfast;
    else if (mealTypeS == "lunch")     mealType = domain::MealType::Lunch;
    else if (mealTypeS == "dinner")    mealType = domain::MealType::Dinner;
    else                               mealType = domain::MealType::Snack;

    if (date.empty() || foodId == 0 || quantityG <= 0) {
        (*cb)(makeBadRequest("date, food_id and quantity_g are required"));
        return;
    }

    useCase->execute(userId, date, mealType, foodId, quantityG,
        [cb](domain::MealEntry e) { (*cb)(makeJsonResponse(serializeMealEntry(e), drogon::k201Created)); },
        [cb](std::string err)     { (*cb)(makeServerError(err)); });
}

void DailyLogController::updateMeal(
    const drogon::HttpRequestPtr& req,
    const std::shared_ptr<RespCb>& cb,
    int64_t entryId, int64_t logId,
    std::shared_ptr<application::UpdateMealEntryUseCase> useCase)
{
    auto body = req->jsonObject();
    if (!body) { (*cb)(makeBadRequest("Request body must be JSON")); return; }

    std::string mealTypeS = (*body).get("meal_type", "snack").asString();
    domain::MealType mealType;
    if      (mealTypeS == "breakfast") mealType = domain::MealType::Breakfast;
    else if (mealTypeS == "lunch")     mealType = domain::MealType::Lunch;
    else if (mealTypeS == "dinner")    mealType = domain::MealType::Dinner;
    else                               mealType = domain::MealType::Snack;

    domain::MealEntry entry{};
    entry.id           = entryId;
    entry.daily_log_id = logId;
    entry.meal_type    = mealType;
    entry.food_id      = static_cast<int64_t>((*body).get("food_id", 0).asInt64());
    entry.quantity_g   = (*body).get("quantity_g", 0.0).asDouble();

    useCase->execute(std::move(entry),
        [cb](domain::MealEntry e) { (*cb)(makeJsonResponse(serializeMealEntry(e))); },
        [cb](std::string err)     { (*cb)(makeServerError(err)); });
}

void DailyLogController::deleteMeal(
    const drogon::HttpRequestPtr&,
    const std::shared_ptr<RespCb>& cb,
    int64_t entryId, int64_t logId,
    std::shared_ptr<application::DeleteMealEntryUseCase> useCase)
{
    useCase->execute(entryId, logId,
        [cb]()            { (*cb)(makeNoContent()); },
        [cb](std::string err) { (*cb)(makeServerError(err)); });
}

} // namespace presentation
