#include "presentation/controllers/ProfileController.hpp"
#include "presentation/Serializers.hpp"

namespace presentation {

void ProfileController::getProfile(
    const drogon::HttpRequestPtr&,
    const std::shared_ptr<RespCb>& cb,
    int64_t userId,
    std::shared_ptr<application::GetProfileUseCase> useCase)
{
    useCase->execute(userId,
        [cb](std::optional<domain::UserProfile> p) {
            if (!p) { (*cb)(makeNotFound("Profile not set")); return; }
            (*cb)(makeJsonResponse(serializeProfile(*p)));
        },
        [cb](std::string err) { (*cb)(makeServerError(err)); });
}

void ProfileController::upsertProfile(
    const drogon::HttpRequestPtr& req,
    const std::shared_ptr<RespCb>& cb,
    int64_t userId,
    std::shared_ptr<application::UpsertProfileUseCase> useCase)
{
    auto body = req->jsonObject();
    if (!body) { (*cb)(makeBadRequest("Request body must be JSON")); return; }

    domain::UserProfile profile{};
    profile.user_id    = userId;
    profile.birth_date = (*body).get("birth_date", "").asString();
    profile.height_cm  = (*body).get("height_cm", 0.0).asDouble();

    auto gStr = (*body).get("gender", "male").asString();
    profile.gender = (gStr == "female") ? domain::Gender::Female : domain::Gender::Male;

    auto alStr = (*body).get("activity_level", "sedentary").asString();
    if      (alStr == "lightly_active")    profile.activity_level = domain::ActivityLevel::LightlyActive;
    else if (alStr == "moderately_active") profile.activity_level = domain::ActivityLevel::ModeratelyActive;
    else if (alStr == "very_active")       profile.activity_level = domain::ActivityLevel::VeryActive;
    else if (alStr == "extra_active")      profile.activity_level = domain::ActivityLevel::ExtraActive;
    else                                   profile.activity_level = domain::ActivityLevel::Sedentary;

    if (profile.birth_date.empty() || profile.height_cm <= 0) {
        (*cb)(makeBadRequest("birth_date and height_cm are required"));
        return;
    }

    useCase->execute(std::move(profile),
        [cb](domain::UserProfile p) { (*cb)(makeJsonResponse(serializeProfile(p))); },
        [cb](std::string err)       { (*cb)(makeServerError(err)); });
}

} // namespace presentation
