#pragma once
#include <json/json.h>
#include <drogon/HttpResponse.h>
#include "domain/entities/User.hpp"
#include "domain/entities/Session.hpp"
#include "domain/entities/UserProfile.hpp"
#include "domain/entities/UserGoal.hpp"
#include "domain/entities/WeightLog.hpp"
#include "domain/entities/BodyMeasurement.hpp"
#include "domain/entities/Food.hpp"
#include "domain/entities/DailyLog.hpp"
#include "domain/entities/MealEntry.hpp"
#include "domain/entities/Exercise.hpp"
#include "domain/entities/ExerciseLog.hpp"
#include "domain/enums/Gender.hpp"
#include "domain/enums/ActivityLevel.hpp"
#include "domain/enums/GoalType.hpp"
#include "domain/enums/MealType.hpp"
#include "domain/enums/ExerciseCategory.hpp"

namespace presentation {

// ── Enum helpers ─────────────────────────────────────────────────────────────

inline std::string genderStr(domain::Gender g) {
    return g == domain::Gender::Male ? "male" : "female";
}

inline std::string activityStr(domain::ActivityLevel a) {
    switch (a) {
        case domain::ActivityLevel::Sedentary:        return "sedentary";
        case domain::ActivityLevel::LightlyActive:    return "lightly_active";
        case domain::ActivityLevel::ModeratelyActive: return "moderately_active";
        case domain::ActivityLevel::VeryActive:       return "very_active";
        default:                                      return "extra_active";
    }
}

inline std::string goalTypeStr(domain::GoalType g) {
    switch (g) {
        case domain::GoalType::LoseWeight: return "lose_weight";
        case domain::GoalType::GainWeight: return "gain_weight";
        default:                           return "maintain";
    }
}

inline std::string mealTypeStr(domain::MealType m) {
    switch (m) {
        case domain::MealType::Breakfast: return "breakfast";
        case domain::MealType::Lunch:     return "lunch";
        case domain::MealType::Dinner:    return "dinner";
        default:                          return "snack";
    }
}

inline std::string exerciseCategoryStr(domain::ExerciseCategory c) {
    switch (c) {
        case domain::ExerciseCategory::Cardio:      return "cardio";
        case domain::ExerciseCategory::Strength:    return "strength";
        case domain::ExerciseCategory::Flexibility: return "flexibility";
        case domain::ExerciseCategory::Sports:      return "sports";
        default:                                    return "other";
    }
}

// ── Entity serializers ────────────────────────────────────────────────────────

inline Json::Value serializeUser(const domain::User& u) {
    Json::Value j;
    j["id"]              = static_cast<Json::Int64>(u.id);
    j["email"]           = u.email;
    j["name"]            = u.name;
    j["profile_picture"] = u.profile_picture.empty() ? Json::Value() : Json::Value(u.profile_picture);
    j["email_verified"]  = u.email_verified;
    j["created_at"]      = u.created_at;
    j["updated_at"]      = u.updated_at;
    return j;
}

inline Json::Value serializeSession(const domain::Session& s) {
    Json::Value j;
    j["token"]      = s.token;
    j["expires_at"] = s.expires_at;
    return j;
}

inline Json::Value serializeProfile(const domain::UserProfile& p) {
    Json::Value j;
    j["id"]             = static_cast<Json::Int64>(p.id);
    j["user_id"]        = static_cast<Json::Int64>(p.user_id);
    j["birth_date"]     = p.birth_date;
    j["gender"]         = genderStr(p.gender);
    j["height_cm"]      = p.height_cm;
    j["activity_level"] = activityStr(p.activity_level);
    j["created_at"]     = p.created_at;
    j["updated_at"]     = p.updated_at;
    return j;
}

inline Json::Value serializeGoal(const domain::UserGoal& g) {
    Json::Value j;
    j["id"]                   = static_cast<Json::Int64>(g.id);
    j["user_id"]              = static_cast<Json::Int64>(g.user_id);
    j["goal_type"]            = goalTypeStr(g.goal_type);
    j["target_weight_kg"]     = g.target_weight_kg;
    j["weekly_rate_kg"]       = g.weekly_rate_kg;
    j["daily_calorie_target"] = g.daily_calorie_target;
    j["start_date"]           = g.start_date;
    j["end_date"]             = g.end_date.empty() ? Json::Value() : Json::Value(g.end_date);
    j["is_active"]            = g.is_active;
    j["created_at"]           = g.created_at;
    j["updated_at"]           = g.updated_at;
    return j;
}

inline Json::Value serializeWeightLog(const domain::WeightLog& w) {
    Json::Value j;
    j["id"]          = static_cast<Json::Int64>(w.id);
    j["user_id"]     = static_cast<Json::Int64>(w.user_id);
    j["weight_kg"]   = w.weight_kg;
    j["logged_date"] = w.logged_date;
    j["notes"]       = w.notes.empty() ? Json::Value() : Json::Value(w.notes);
    j["created_at"]  = w.created_at;
    return j;
}

inline Json::Value serializeMeasurement(const domain::BodyMeasurement& m) {
    Json::Value j;
    j["id"]                  = static_cast<Json::Int64>(m.id);
    j["user_id"]             = static_cast<Json::Int64>(m.user_id);
    j["measured_date"]       = m.measured_date;
    j["neck_cm"]             = m.neck_cm;
    j["waist_cm"]            = m.waist_cm;
    j["hip_cm"]              = m.hip_cm.has_value()             ? Json::Value(*m.hip_cm)             : Json::Value();
    j["body_fat_percentage"] = m.body_fat_percentage.has_value() ? Json::Value(*m.body_fat_percentage) : Json::Value();
    j["notes"]               = m.notes.empty() ? Json::Value() : Json::Value(m.notes);
    j["created_at"]          = m.created_at;
    return j;
}

inline Json::Value serializeFood(const domain::Food& f) {
    Json::Value j;
    j["id"]                 = static_cast<Json::Int64>(f.id);
    j["name"]               = f.name;
    j["brand"]              = f.brand.empty()   ? Json::Value() : Json::Value(f.brand);
    j["barcode"]            = f.barcode.empty() ? Json::Value() : Json::Value(f.barcode);
    j["serving_size_g"]     = f.serving_size_g;
    j["calories"]           = f.calories;
    j["protein_g"]          = f.protein_g;
    j["carbs_g"]            = f.carbs_g;
    j["fat_g"]              = f.fat_g;
    j["fiber_g"]            = f.fiber_g;
    j["sugar_g"]            = f.sugar_g;
    j["sodium_mg"]          = f.sodium_mg;
    j["created_by_user_id"] = f.created_by_user_id == 0
                                ? Json::Value()
                                : Json::Value(static_cast<Json::Int64>(f.created_by_user_id));
    j["is_verified"]        = f.is_verified;
    j["created_at"]         = f.created_at;
    j["updated_at"]         = f.updated_at;
    return j;
}

inline Json::Value serializeDailyLog(const domain::DailyLog& d) {
    Json::Value j;
    j["id"]         = static_cast<Json::Int64>(d.id);
    j["user_id"]    = static_cast<Json::Int64>(d.user_id);
    j["log_date"]   = d.log_date;
    j["water_ml"]   = d.water_ml;
    j["notes"]      = d.notes.empty() ? Json::Value() : Json::Value(d.notes);
    j["created_at"] = d.created_at;
    j["updated_at"] = d.updated_at;
    return j;
}

inline Json::Value serializeMealEntry(const domain::MealEntry& m) {
    Json::Value j;
    j["id"]           = static_cast<Json::Int64>(m.id);
    j["daily_log_id"] = static_cast<Json::Int64>(m.daily_log_id);
    j["meal_type"]    = mealTypeStr(m.meal_type);
    j["food_id"]      = static_cast<Json::Int64>(m.food_id);
    j["quantity_g"]   = m.quantity_g;
    j["calories"]     = m.calories;
    j["protein_g"]    = m.protein_g;
    j["carbs_g"]      = m.carbs_g;
    j["fat_g"]        = m.fat_g;
    j["created_at"]   = m.created_at;
    return j;
}

inline Json::Value serializeExercise(const domain::Exercise& e) {
    Json::Value j;
    j["id"]                 = static_cast<Json::Int64>(e.id);
    j["name"]               = e.name;
    j["category"]           = exerciseCategoryStr(e.category);
    j["met_value"]          = e.met_value;
    j["description"]        = e.description.empty() ? Json::Value() : Json::Value(e.description);
    j["created_by_user_id"] = e.created_by_user_id == 0
                                ? Json::Value()
                                : Json::Value(static_cast<Json::Int64>(e.created_by_user_id));
    j["is_verified"]        = e.is_verified;
    j["created_at"]         = e.created_at;
    return j;
}

inline Json::Value serializeExerciseLog(const domain::ExerciseLog& e) {
    Json::Value j;
    j["id"]               = static_cast<Json::Int64>(e.id);
    j["user_id"]          = static_cast<Json::Int64>(e.user_id);
    j["exercise_id"]      = static_cast<Json::Int64>(e.exercise_id);
    j["log_date"]         = e.log_date;
    j["duration_minutes"] = e.duration_minutes;
    j["calories_burned"]  = e.calories_burned;
    j["sets"]             = e.sets.has_value()           ? Json::Value(*e.sets)           : Json::Value();
    j["reps"]             = e.reps.has_value()           ? Json::Value(*e.reps)           : Json::Value();
    j["distance_km"]      = e.distance_km.has_value()    ? Json::Value(*e.distance_km)    : Json::Value();
    j["avg_heart_rate"]   = e.avg_heart_rate.has_value() ? Json::Value(*e.avg_heart_rate) : Json::Value();
    j["notes"]            = e.notes.empty() ? Json::Value() : Json::Value(e.notes);
    j["created_at"]       = e.created_at;
    return j;
}

// ── Response builders ─────────────────────────────────────────────────────────

inline drogon::HttpResponsePtr makeError(drogon::HttpStatusCode code, const std::string& msg) {
    Json::Value j;
    j["error"] = msg;
    auto resp = drogon::HttpResponse::newHttpJsonResponse(j);
    resp->setStatusCode(code);
    return resp;
}

inline drogon::HttpResponsePtr makeBadRequest(const std::string& msg) {
    return makeError(drogon::k400BadRequest, msg);
}

inline drogon::HttpResponsePtr makeUnauthorized(const std::string& msg = "Unauthorized") {
    return makeError(drogon::k401Unauthorized, msg);
}

inline drogon::HttpResponsePtr makeForbidden(const std::string& msg = "Forbidden") {
    return makeError(drogon::k403Forbidden, msg);
}

inline drogon::HttpResponsePtr makeNotFound(const std::string& msg = "Not found") {
    return makeError(drogon::k404NotFound, msg);
}

inline drogon::HttpResponsePtr makeServerError(const std::string& msg = "Internal server error") {
    return makeError(drogon::k500InternalServerError, msg);
}

inline drogon::HttpResponsePtr makeJsonResponse(const Json::Value& body,
                                                drogon::HttpStatusCode code = drogon::k200OK) {
    auto resp = drogon::HttpResponse::newHttpJsonResponse(body);
    resp->setStatusCode(code);
    return resp;
}

inline drogon::HttpResponsePtr makeNoContent() {
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::k204NoContent);
    return resp;
}

} // namespace presentation
