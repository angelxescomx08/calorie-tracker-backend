#include "infrastructure/persistence/PostgresUserGoalRepository.hpp"

namespace infrastructure {

PostgresUserGoalRepository::PostgresUserGoalRepository(drogon::orm::DbClientPtr db)
    : db_(std::move(db)) {}

domain::UserGoal PostgresUserGoalRepository::rowToGoal(const drogon::orm::Row& row)
{
    domain::UserGoal g;
    g.id                   = row["id"].as<int64_t>();
    g.user_id              = row["user_id"].as<int64_t>();
    g.target_weight_kg     = row["target_weight_kg"].as<double>();
    g.weekly_rate_kg       = row["weekly_rate_kg"].as<double>();
    g.daily_calorie_target = row["daily_calorie_target"].as<int>();
    g.start_date           = row["start_date"].as<std::string>();
    g.end_date             = row["end_date"].isNull() ? "" : row["end_date"].as<std::string>();
    g.is_active            = row["is_active"].as<bool>();
    g.created_at           = row["created_at"].as<std::string>();
    g.updated_at           = row["updated_at"].as<std::string>();

    auto gt = row["goal_type"].as<std::string>();
    if      (gt == "lose_weight") g.goal_type = domain::GoalType::LoseWeight;
    else if (gt == "gain_weight") g.goal_type = domain::GoalType::GainWeight;
    else                          g.goal_type = domain::GoalType::Maintain;

    return g;
}

void PostgresUserGoalRepository::findActiveByUserId(
    int64_t userId,
    std::function<void(std::optional<domain::UserGoal>)> onSuccess,
    std::function<void(std::string)>                     onError)
{
    auto self = shared_from_this();
    db_->execSqlAsync(
        "SELECT id, user_id, goal_type, target_weight_kg, weekly_rate_kg, daily_calorie_target,"
        " start_date::text, end_date::text, is_active, created_at::text, updated_at::text"
        " FROM user_goals WHERE user_id = $1 AND is_active = TRUE LIMIT 1",
        [self, onSuccess = std::move(onSuccess)](const drogon::orm::Result& r) {
            if (r.empty()) { onSuccess(std::nullopt); return; }
            onSuccess(rowToGoal(r[0]));
        },
        [onError = std::move(onError)](const drogon::orm::DrogonDbException& e) {
            onError(e.base().what());
        },
        userId);
}

void PostgresUserGoalRepository::create(
    domain::UserGoal goal,
    std::function<void(domain::UserGoal)>    onSuccess,
    std::function<void(std::string)>         onError)
{
    static const char* kGoalTypes[] = { "lose_weight", "gain_weight", "maintain" };
    std::string gtStr = kGoalTypes[static_cast<int>(goal.goal_type)];

    // Deactivate previous active goals first, then create new one.
    auto self = shared_from_this();
    auto onSuccessPtr = std::make_shared<decltype(onSuccess)>(std::move(onSuccess));
    auto onErrorPtr   = std::make_shared<decltype(onError)>(std::move(onError));

    db_->execSqlAsync(
        "UPDATE user_goals SET is_active = FALSE WHERE user_id = $1 AND is_active = TRUE",
        [self, goal, gtStr, onSuccessPtr, onErrorPtr](const drogon::orm::Result&) {
            std::string endDate = goal.end_date.empty() ? "NULL" : ("'" + goal.end_date + "'");
            std::string sql =
                "INSERT INTO user_goals"
                " (user_id, goal_type, target_weight_kg, weekly_rate_kg, daily_calorie_target,"
                "  start_date, end_date, is_active)"
                " VALUES ($1, $2::goal_type, $3, $4, $5, $6, " + endDate + ", TRUE)"
                " RETURNING id, user_id, goal_type, target_weight_kg, weekly_rate_kg,"
                " daily_calorie_target, start_date::text, end_date::text, is_active,"
                " created_at::text, updated_at::text";

            self->db_->execSqlAsync(
                sql,
                [onSuccessPtr](const drogon::orm::Result& r) {
                    (*onSuccessPtr)(rowToGoal(r[0]));
                },
                [onErrorPtr](const drogon::orm::DrogonDbException& e) {
                    (*onErrorPtr)(e.base().what());
                },
                goal.user_id, gtStr, goal.target_weight_kg,
                goal.weekly_rate_kg, goal.daily_calorie_target, goal.start_date);
        },
        [onErrorPtr](const drogon::orm::DrogonDbException& e) {
            (*onErrorPtr)(e.base().what());
        },
        goal.user_id);
}

void PostgresUserGoalRepository::update(
    domain::UserGoal goal,
    std::function<void(domain::UserGoal)>    onSuccess,
    std::function<void(std::string)>         onError)
{
    static const char* kGoalTypes[] = { "lose_weight", "gain_weight", "maintain" };
    std::string gtStr = kGoalTypes[static_cast<int>(goal.goal_type)];
    std::string endDate = goal.end_date.empty() ? "NULL" : ("'" + goal.end_date + "'");

    std::string sql =
        "UPDATE user_goals SET"
        " goal_type = $1::goal_type, target_weight_kg = $2, weekly_rate_kg = $3,"
        " daily_calorie_target = $4, start_date = $5, end_date = " + endDate + ","
        " updated_at = NOW()"
        " WHERE id = $6 AND user_id = $7"
        " RETURNING id, user_id, goal_type, target_weight_kg, weekly_rate_kg,"
        " daily_calorie_target, start_date::text, end_date::text, is_active,"
        " created_at::text, updated_at::text";

    auto self = shared_from_this();
    db_->execSqlAsync(
        sql,
        [self, onSuccess = std::move(onSuccess), onError]
        (const drogon::orm::Result& r) {
            if (r.empty()) { onError("Goal not found or access denied"); return; }
            onSuccess(rowToGoal(r[0]));
        },
        [onError = std::move(onError)](const drogon::orm::DrogonDbException& e) {
            onError(e.base().what());
        },
        gtStr, goal.target_weight_kg, goal.weekly_rate_kg,
        goal.daily_calorie_target, goal.start_date, goal.id, goal.user_id);
}

void PostgresUserGoalRepository::deactivate(
    int64_t id, int64_t userId,
    std::function<void()>            onSuccess,
    std::function<void(std::string)> onError)
{
    auto self = shared_from_this();
    db_->execSqlAsync(
        "UPDATE user_goals SET is_active = FALSE, updated_at = NOW() "
        "WHERE id = $1 AND user_id = $2",
        [self, onSuccess = std::move(onSuccess)](const drogon::orm::Result&) {
            onSuccess();
        },
        [onError = std::move(onError)](const drogon::orm::DrogonDbException& e) {
            onError(e.base().what());
        },
        id, userId);
}

} // namespace infrastructure
