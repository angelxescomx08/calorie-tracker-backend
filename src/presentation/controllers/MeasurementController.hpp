#pragma once
#include <memory>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include "presentation/middleware/AuthMiddleware.hpp"
#include "application/use_cases/measurements/ListMeasurementsUseCase.hpp"
#include "application/use_cases/measurements/CreateMeasurementUseCase.hpp"
#include "application/use_cases/measurements/DeleteMeasurementUseCase.hpp"

namespace presentation {

class MeasurementController {
public:
    static void list(const drogon::HttpRequestPtr& req, const std::shared_ptr<RespCb>& cb,
        int64_t userId, std::shared_ptr<application::ListMeasurementsUseCase> useCase);

    static void create(const drogon::HttpRequestPtr& req, const std::shared_ptr<RespCb>& cb,
        int64_t userId, std::shared_ptr<application::CreateMeasurementUseCase> useCase);

    static void remove(const drogon::HttpRequestPtr& req, const std::shared_ptr<RespCb>& cb,
        int64_t id, int64_t userId, std::shared_ptr<application::DeleteMeasurementUseCase> useCase);
};

} // namespace presentation
