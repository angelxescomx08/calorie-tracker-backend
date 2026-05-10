#include "presentation/controllers/WeightController.hpp"
#include "presentation/Serializers.hpp"

namespace presentation {

void WeightController::list(
    const drogon::HttpRequestPtr& req,
    const std::shared_ptr<RespCb>& cb,
    int64_t userId,
    std::shared_ptr<application::ListWeightLogsUseCase> useCase)
{
    std::string startDate = req->getParameter("start_date");
    std::string endDate   = req->getParameter("end_date");
    int limit = 100;
    auto limitStr = req->getParameter("limit");
    if (!limitStr.empty()) limit = std::stoi(limitStr);

    useCase->execute(userId, startDate, endDate, limit,
        [cb](std::vector<domain::WeightLog> logs) {
            Json::Value arr(Json::arrayValue);
            for (const auto& l : logs) arr.append(serializeWeightLog(l));
            Json::Value j;
            j["items"] = arr;
            (*cb)(makeJsonResponse(j));
        },
        [cb](std::string err) { (*cb)(makeServerError(err)); });
}

void WeightController::create(
    const drogon::HttpRequestPtr& req,
    const std::shared_ptr<RespCb>& cb,
    int64_t userId,
    std::shared_ptr<application::CreateWeightLogUseCase> useCase)
{
    auto body = req->jsonObject();
    if (!body) { (*cb)(makeBadRequest("Request body must be JSON")); return; }

    domain::WeightLog log{};
    log.user_id     = userId;
    log.weight_kg   = (*body).get("weight_kg", 0.0).asDouble();
    log.logged_date = (*body).get("logged_date", "").asString();
    log.notes       = (*body).get("notes", "").asString();

    if (log.weight_kg <= 0 || log.logged_date.empty()) {
        (*cb)(makeBadRequest("weight_kg and logged_date are required"));
        return;
    }

    useCase->execute(std::move(log),
        [cb](domain::WeightLog l) { (*cb)(makeJsonResponse(serializeWeightLog(l), drogon::k201Created)); },
        [cb](std::string err)     { (*cb)(makeServerError(err)); });
}

void WeightController::update(
    const drogon::HttpRequestPtr& req,
    const std::shared_ptr<RespCb>& cb,
    int64_t logId, int64_t userId,
    std::shared_ptr<application::UpdateWeightLogUseCase> useCase)
{
    auto body = req->jsonObject();
    if (!body) { (*cb)(makeBadRequest("Request body must be JSON")); return; }

    domain::WeightLog log{};
    log.id          = logId;
    log.user_id     = userId;
    log.weight_kg   = (*body).get("weight_kg", 0.0).asDouble();
    log.logged_date = (*body).get("logged_date", "").asString();
    log.notes       = (*body).get("notes", "").asString();

    useCase->execute(std::move(log),
        [cb](domain::WeightLog l) { (*cb)(makeJsonResponse(serializeWeightLog(l))); },
        [cb](std::string err)     { (*cb)(makeServerError(err)); });
}

void WeightController::remove(
    const drogon::HttpRequestPtr&,
    const std::shared_ptr<RespCb>& cb,
    int64_t logId, int64_t userId,
    std::shared_ptr<application::DeleteWeightLogUseCase> useCase)
{
    useCase->execute(logId, userId,
        [cb]()            { (*cb)(makeNoContent()); },
        [cb](std::string err) { (*cb)(makeServerError(err)); });
}

} // namespace presentation
