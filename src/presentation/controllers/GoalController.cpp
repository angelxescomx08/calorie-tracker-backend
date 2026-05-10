#include "presentation/controllers/GoalController.hpp"
#include "presentation/Serializers.hpp"

namespace presentation {

static domain::GoalType parseGoalType(const std::string& s) {
    if (s == "gain_weight") return domain::GoalType::GainWeight;
    if (s == "maintain")    return domain::GoalType::Maintain;
    return domain::GoalType::LoseWeight;
}

void GoalController::getActive(
    const drogon::HttpRequestPtr&,
    const std::shared_ptr<RespCb>& cb,
    int64_t userId,
    std::shared_ptr<application::GetActiveGoalUseCase> useCase)
{
    useCase->execute(userId,
        [cb](std::optional<domain::UserGoal> g) {
            if (!g) { (*cb)(makeNotFound("No active goal")); return; }
            (*cb)(makeJsonResponse(serializeGoal(*g)));
        },
        [cb](std::string err) { (*cb)(makeServerError(err)); });
}

void GoalController::create(
    const drogon::HttpRequestPtr& req,
    const std::shared_ptr<RespCb>& cb,
    int64_t userId,
    std::shared_ptr<application::CreateGoalUseCase> useCase)
{
    auto body = req->jsonObject();
    if (!body) { (*cb)(makeBadRequest("Request body must be JSON")); return; }

    domain::UserGoal goal{};
    goal.user_id              = userId;
    goal.goal_type            = parseGoalType((*body).get("goal_type", "lose_weight").asString());
    goal.target_weight_kg     = (*body).get("target_weight_kg", 0.0).asDouble();
    goal.weekly_rate_kg       = (*body).get("weekly_rate_kg", 0.5).asDouble();
    goal.daily_calorie_target = (*body).get("daily_calorie_target", 2000).asInt();
    goal.start_date           = (*body).get("start_date", "").asString();
    goal.end_date             = (*body).get("end_date", "").asString();
    goal.is_active            = true;

    if (goal.target_weight_kg <= 0 || goal.start_date.empty()) {
        (*cb)(makeBadRequest("target_weight_kg and start_date are required"));
        return;
    }

    useCase->execute(std::move(goal),
        [cb](domain::UserGoal g) { (*cb)(makeJsonResponse(serializeGoal(g), drogon::k201Created)); },
        [cb](std::string err)    { (*cb)(makeServerError(err)); });
}

void GoalController::update(
    const drogon::HttpRequestPtr& req,
    const std::shared_ptr<RespCb>& cb,
    int64_t goalId, int64_t userId,
    std::shared_ptr<application::UpdateGoalUseCase> useCase)
{
    auto body = req->jsonObject();
    if (!body) { (*cb)(makeBadRequest("Request body must be JSON")); return; }

    domain::UserGoal goal{};
    goal.id                   = goalId;
    goal.user_id              = userId;
    goal.goal_type            = parseGoalType((*body).get("goal_type", "lose_weight").asString());
    goal.target_weight_kg     = (*body).get("target_weight_kg", 0.0).asDouble();
    goal.weekly_rate_kg       = (*body).get("weekly_rate_kg", 0.5).asDouble();
    goal.daily_calorie_target = (*body).get("daily_calorie_target", 2000).asInt();
    goal.start_date           = (*body).get("start_date", "").asString();
    goal.end_date             = (*body).get("end_date", "").asString();

    useCase->execute(std::move(goal),
        [cb](domain::UserGoal g) { (*cb)(makeJsonResponse(serializeGoal(g))); },
        [cb](std::string err)    { (*cb)(makeServerError(err)); });
}

void GoalController::deactivate(
    const drogon::HttpRequestPtr&,
    const std::shared_ptr<RespCb>& cb,
    int64_t goalId, int64_t userId,
    std::shared_ptr<application::DeactivateGoalUseCase> useCase)
{
    useCase->execute(goalId, userId,
        [cb]()            { (*cb)(makeNoContent()); },
        [cb](std::string err) { (*cb)(makeServerError(err)); });
}

} // namespace presentation
