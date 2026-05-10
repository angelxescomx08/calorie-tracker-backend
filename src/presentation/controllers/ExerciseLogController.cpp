#include "presentation/controllers/ExerciseLogController.hpp"
#include "presentation/Serializers.hpp"

namespace presentation {

void ExerciseLogController::list(
    const drogon::HttpRequestPtr& req,
    const std::shared_ptr<RespCb>& cb,
    int64_t userId,
    std::shared_ptr<application::ListExerciseLogsUseCase> useCase)
{
    std::string date = req->getParameter("date");
    if (date.empty()) {
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
        [cb](std::vector<domain::ExerciseLog> logs) {
            Json::Value arr(Json::arrayValue);
            for (const auto& l : logs) arr.append(serializeExerciseLog(l));
            Json::Value j;
            j["items"] = arr;
            (*cb)(makeJsonResponse(j));
        },
        [cb](std::string err) { (*cb)(makeServerError(err)); });
}

void ExerciseLogController::create(
    const drogon::HttpRequestPtr& req,
    const std::shared_ptr<RespCb>& cb,
    int64_t userId,
    std::shared_ptr<application::CreateExerciseLogUseCase> useCase)
{
    auto body = req->jsonObject();
    if (!body) { (*cb)(makeBadRequest("Request body must be JSON")); return; }

    domain::ExerciseLog log{};
    log.user_id          = userId;
    log.exercise_id      = static_cast<int64_t>((*body).get("exercise_id", 0).asInt64());
    log.log_date         = (*body).get("log_date", "").asString();
    log.duration_minutes = (*body).get("duration_minutes", 0).asInt();
    log.notes            = (*body).get("notes", "").asString();

    if ((*body).isMember("sets") && !(*body)["sets"].isNull())
        log.sets = (*body)["sets"].asInt();
    if ((*body).isMember("reps") && !(*body)["reps"].isNull())
        log.reps = (*body)["reps"].asInt();
    if ((*body).isMember("distance_km") && !(*body)["distance_km"].isNull())
        log.distance_km = (*body)["distance_km"].asDouble();
    if ((*body).isMember("avg_heart_rate") && !(*body)["avg_heart_rate"].isNull())
        log.avg_heart_rate = (*body)["avg_heart_rate"].asInt();

    if (log.exercise_id == 0 || log.log_date.empty() || log.duration_minutes <= 0) {
        (*cb)(makeBadRequest("exercise_id, log_date and duration_minutes are required"));
        return;
    }

    useCase->execute(std::move(log),
        [cb](domain::ExerciseLog l) { (*cb)(makeJsonResponse(serializeExerciseLog(l), drogon::k201Created)); },
        [cb](std::string err)       { (*cb)(makeServerError(err)); });
}

void ExerciseLogController::update(
    const drogon::HttpRequestPtr& req,
    const std::shared_ptr<RespCb>& cb,
    int64_t logId, int64_t userId,
    std::shared_ptr<application::UpdateExerciseLogUseCase> useCase)
{
    auto body = req->jsonObject();
    if (!body) { (*cb)(makeBadRequest("Request body must be JSON")); return; }

    domain::ExerciseLog log{};
    log.id               = logId;
    log.user_id          = userId;
    log.exercise_id      = static_cast<int64_t>((*body).get("exercise_id", 0).asInt64());
    log.log_date         = (*body).get("log_date", "").asString();
    log.duration_minutes = (*body).get("duration_minutes", 0).asInt();
    log.calories_burned  = (*body).get("calories_burned", 0.0).asDouble();
    log.notes            = (*body).get("notes", "").asString();

    if ((*body).isMember("sets") && !(*body)["sets"].isNull())
        log.sets = (*body)["sets"].asInt();
    if ((*body).isMember("reps") && !(*body)["reps"].isNull())
        log.reps = (*body)["reps"].asInt();
    if ((*body).isMember("distance_km") && !(*body)["distance_km"].isNull())
        log.distance_km = (*body)["distance_km"].asDouble();
    if ((*body).isMember("avg_heart_rate") && !(*body)["avg_heart_rate"].isNull())
        log.avg_heart_rate = (*body)["avg_heart_rate"].asInt();

    useCase->execute(std::move(log),
        [cb](domain::ExerciseLog l) { (*cb)(makeJsonResponse(serializeExerciseLog(l))); },
        [cb](std::string err)       { (*cb)(makeServerError(err)); });
}

void ExerciseLogController::remove(
    const drogon::HttpRequestPtr&,
    const std::shared_ptr<RespCb>& cb,
    int64_t logId, int64_t userId,
    std::shared_ptr<application::DeleteExerciseLogUseCase> useCase)
{
    useCase->execute(logId, userId,
        [cb]()            { (*cb)(makeNoContent()); },
        [cb](std::string err) { (*cb)(makeServerError(err)); });
}

} // namespace presentation
