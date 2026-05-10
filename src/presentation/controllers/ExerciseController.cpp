#include "presentation/controllers/ExerciseController.hpp"
#include "presentation/Serializers.hpp"

namespace presentation {

static domain::ExerciseCategory parseCategory(const std::string& s) {
    if (s == "strength")    return domain::ExerciseCategory::Strength;
    if (s == "flexibility") return domain::ExerciseCategory::Flexibility;
    if (s == "sports")      return domain::ExerciseCategory::Sports;
    if (s == "other")       return domain::ExerciseCategory::Other;
    return domain::ExerciseCategory::Cardio;
}

void ExerciseController::search(
    const drogon::HttpRequestPtr& req,
    const std::shared_ptr<RespCb>& cb,
    std::shared_ptr<application::SearchExercisesUseCase> useCase)
{
    std::string query    = req->getParameter("q");
    std::string category = req->getParameter("category");
    int limit  = 20;
    int offset = 0;
    auto ls = req->getParameter("limit");
    auto os = req->getParameter("offset");
    if (!ls.empty()) limit  = std::stoi(ls);
    if (!os.empty()) offset = std::stoi(os);

    useCase->execute(query, category, limit, offset,
        [cb, limit, offset](std::vector<domain::Exercise> exs, int total) {
            Json::Value arr(Json::arrayValue);
            for (const auto& e : exs) arr.append(serializeExercise(e));
            Json::Value j;
            j["items"]  = arr;
            j["total"]  = total;
            j["limit"]  = limit;
            j["offset"] = offset;
            (*cb)(makeJsonResponse(j));
        },
        [cb](std::string err) { (*cb)(makeServerError(err)); });
}

void ExerciseController::getOne(
    const drogon::HttpRequestPtr&,
    const std::shared_ptr<RespCb>& cb,
    int64_t exerciseId,
    std::shared_ptr<application::GetExerciseUseCase> useCase)
{
    useCase->execute(exerciseId,
        [cb](std::optional<domain::Exercise> e) {
            if (!e) { (*cb)(makeNotFound("Exercise not found")); return; }
            (*cb)(makeJsonResponse(serializeExercise(*e)));
        },
        [cb](std::string err) { (*cb)(makeServerError(err)); });
}

void ExerciseController::create(
    const drogon::HttpRequestPtr& req,
    const std::shared_ptr<RespCb>& cb,
    int64_t userId,
    std::shared_ptr<application::CreateExerciseUseCase> useCase)
{
    auto body = req->jsonObject();
    if (!body) { (*cb)(makeBadRequest("Request body must be JSON")); return; }

    domain::Exercise e{};
    e.name               = (*body).get("name", "").asString();
    e.category           = parseCategory((*body).get("category", "cardio").asString());
    e.met_value          = (*body).get("met_value", 0.0).asDouble();
    e.description        = (*body).get("description", "").asString();
    e.created_by_user_id = userId;
    e.is_verified        = false;

    if (e.name.empty() || e.met_value <= 0) {
        (*cb)(makeBadRequest("name and met_value are required"));
        return;
    }

    useCase->execute(std::move(e),
        [cb](domain::Exercise ex) { (*cb)(makeJsonResponse(serializeExercise(ex), drogon::k201Created)); },
        [cb](std::string err)     { (*cb)(makeServerError(err)); });
}

} // namespace presentation
