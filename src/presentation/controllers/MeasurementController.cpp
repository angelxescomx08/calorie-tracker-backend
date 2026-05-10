#include "presentation/controllers/MeasurementController.hpp"
#include "presentation/Serializers.hpp"

namespace presentation {

void MeasurementController::list(
    const drogon::HttpRequestPtr& req,
    const std::shared_ptr<RespCb>& cb,
    int64_t userId,
    std::shared_ptr<application::ListMeasurementsUseCase> useCase)
{
    std::string startDate = req->getParameter("start_date");
    std::string endDate   = req->getParameter("end_date");

    useCase->execute(userId, startDate, endDate,
        [cb](std::vector<domain::BodyMeasurement> ms) {
            Json::Value arr(Json::arrayValue);
            for (const auto& m : ms) arr.append(serializeMeasurement(m));
            Json::Value j;
            j["items"] = arr;
            (*cb)(makeJsonResponse(j));
        },
        [cb](std::string err) { (*cb)(makeServerError(err)); });
}

void MeasurementController::create(
    const drogon::HttpRequestPtr& req,
    const std::shared_ptr<RespCb>& cb,
    int64_t userId,
    std::shared_ptr<application::CreateMeasurementUseCase> useCase)
{
    auto body = req->jsonObject();
    if (!body) { (*cb)(makeBadRequest("Request body must be JSON")); return; }

    domain::BodyMeasurement m{};
    m.user_id       = userId;
    m.measured_date = (*body).get("measured_date", "").asString();
    m.neck_cm       = (*body).get("neck_cm", 0.0).asDouble();
    m.waist_cm      = (*body).get("waist_cm", 0.0).asDouble();
    m.notes         = (*body).get("notes", "").asString();

    if ((*body).isMember("hip_cm") && !(*body)["hip_cm"].isNull())
        m.hip_cm = (*body)["hip_cm"].asDouble();

    if (m.measured_date.empty() || m.neck_cm <= 0 || m.waist_cm <= 0) {
        (*cb)(makeBadRequest("measured_date, neck_cm and waist_cm are required"));
        return;
    }

    useCase->execute(std::move(m),
        [cb](domain::BodyMeasurement ms) { (*cb)(makeJsonResponse(serializeMeasurement(ms), drogon::k201Created)); },
        [cb](std::string err)            { (*cb)(makeServerError(err)); });
}

void MeasurementController::remove(
    const drogon::HttpRequestPtr&,
    const std::shared_ptr<RespCb>& cb,
    int64_t id, int64_t userId,
    std::shared_ptr<application::DeleteMeasurementUseCase> useCase)
{
    useCase->execute(id, userId,
        [cb]()            { (*cb)(makeNoContent()); },
        [cb](std::string err) { (*cb)(makeServerError(err)); });
}

} // namespace presentation
