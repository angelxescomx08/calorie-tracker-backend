#include "infrastructure/persistence/PostgresUserProfileRepository.hpp"

namespace infrastructure {

PostgresUserProfileRepository::PostgresUserProfileRepository(drogon::orm::DbClientPtr db)
    : db_(std::move(db)) {}

domain::UserProfile PostgresUserProfileRepository::rowToProfile(const drogon::orm::Row& row)
{
    domain::UserProfile p;
    p.id         = row["id"].as<int64_t>();
    p.user_id    = row["user_id"].as<int64_t>();
    p.birth_date = row["birth_date"].as<std::string>();
    p.height_cm  = row["height_cm"].as<double>();
    p.created_at = row["created_at"].as<std::string>();
    p.updated_at = row["updated_at"].as<std::string>();

    auto g = row["gender"].as<std::string>();
    p.gender = (g == "male") ? domain::Gender::Male : domain::Gender::Female;

    auto al = row["activity_level"].as<std::string>();
    if      (al == "sedentary")         p.activity_level = domain::ActivityLevel::Sedentary;
    else if (al == "lightly_active")    p.activity_level = domain::ActivityLevel::LightlyActive;
    else if (al == "moderately_active") p.activity_level = domain::ActivityLevel::ModeratelyActive;
    else if (al == "very_active")       p.activity_level = domain::ActivityLevel::VeryActive;
    else                                p.activity_level = domain::ActivityLevel::ExtraActive;

    return p;
}

void PostgresUserProfileRepository::findByUserId(
    int64_t userId,
    std::function<void(std::optional<domain::UserProfile>)> onSuccess,
    std::function<void(std::string)>                        onError)
{
    auto self = shared_from_this();
    db_->execSqlAsync(
        "SELECT id, user_id, birth_date::text, gender, height_cm, activity_level, "
        "created_at::text, updated_at::text FROM user_profiles WHERE user_id = $1",
        [self, onSuccess = std::move(onSuccess)](const drogon::orm::Result& r) {
            if (r.empty()) { onSuccess(std::nullopt); return; }
            onSuccess(rowToProfile(r[0]));
        },
        [onError = std::move(onError)](const drogon::orm::DrogonDbException& e) {
            onError(e.base().what());
        },
        userId);
}

void PostgresUserProfileRepository::upsert(
    domain::UserProfile profile,
    std::function<void(domain::UserProfile)> onSuccess,
    std::function<void(std::string)>         onError)
{
    static const char* kGenders[] = { "male", "female" };
    static const char* kLevels[]  = {
        "sedentary", "lightly_active", "moderately_active", "very_active", "extra_active"
    };

    std::string gStr = kGenders[static_cast<int>(profile.gender)];
    std::string lStr = kLevels[static_cast<int>(profile.activity_level)];

    static const char* kSql =
        "INSERT INTO user_profiles (user_id, birth_date, gender, height_cm, activity_level)"
        " VALUES ($1, $2, $3::gender_type, $4, $5::activity_level_type)"
        " ON CONFLICT (user_id) DO UPDATE SET"
        "   birth_date = EXCLUDED.birth_date,"
        "   gender = EXCLUDED.gender,"
        "   height_cm = EXCLUDED.height_cm,"
        "   activity_level = EXCLUDED.activity_level,"
        "   updated_at = NOW()"
        " RETURNING id, user_id, birth_date::text, gender, height_cm, activity_level,"
        "           created_at::text, updated_at::text";

    auto self = shared_from_this();
    db_->execSqlAsync(
        kSql,
        [self, onSuccess = std::move(onSuccess)](const drogon::orm::Result& r) {
            onSuccess(rowToProfile(r[0]));
        },
        [onError = std::move(onError)](const drogon::orm::DrogonDbException& e) {
            onError(e.base().what());
        },
        profile.user_id, profile.birth_date, gStr, profile.height_cm, lStr);
}

} // namespace infrastructure
