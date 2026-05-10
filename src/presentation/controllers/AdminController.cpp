#include "presentation/controllers/AdminController.hpp"
#include <json/json.h>

namespace presentation {

AdminController::AdminController(
    std::shared_ptr<application::SeedExercisesUseCase> seedUseCase
)
    : seedUseCase_(std::move(seedUseCase))
{}

void AdminController::seedExercises(
    const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callbackRv
)
{
    // Convert rvalue ref to named value so it can be copied into two lambdas.
    auto callback = std::move(callbackRv);

    seedUseCase_->execute(
        [callback](int count) {
            Json::Value body;
            body["inserted"] = count;
            body["message"]  = "Exercises seeded successfully";
            callback(drogon::HttpResponse::newHttpJsonResponse(body));
        },
        [callback](const std::string& error) {
            Json::Value body;
            body["error"] = error;
            auto resp = drogon::HttpResponse::newHttpJsonResponse(body);
            resp->setStatusCode(drogon::k500InternalServerError);
            callback(resp);
        }
    );
}

} // namespace presentation
