#include "infrastructure/persistence/PostgresExerciseLogRepository.hpp"

namespace infrastructure {

PostgresExerciseLogRepository::PostgresExerciseLogRepository(drogon::orm::DbClientPtr db)
    : db_(std::move(db)) {}

domain::ExerciseLog PostgresExerciseLogRepository::rowToLog(const drogon::orm::Row& row)
{
    domain::ExerciseLog e;
    e.id               = row["id"].as<int64_t>();
    e.user_id          = row["user_id"].as<int64_t>();
    e.exercise_id      = row["exercise_id"].as<int64_t>();
    e.log_date         = row["log_date"].as<std::string>();
    e.duration_minutes = row["duration_minutes"].as<int>();
    e.calories_burned  = row["calories_burned"].as<double>();
    e.sets             = row["sets"].isNull()           ? std::nullopt : std::optional<int>(row["sets"].as<int>());
    e.reps             = row["reps"].isNull()           ? std::nullopt : std::optional<int>(row["reps"].as<int>());
    e.distance_km      = row["distance_km"].isNull()    ? std::nullopt : std::optional<double>(row["distance_km"].as<double>());
    e.avg_heart_rate   = row["avg_heart_rate"].isNull() ? std::nullopt : std::optional<int>(row["avg_heart_rate"].as<int>());
    e.notes            = row["notes"].isNull() ? "" : row["notes"].as<std::string>();
    e.created_at       = row["created_at"].as<std::string>();
    return e;
}

void PostgresExerciseLogRepository::listByUserAndDate(
    int64_t userId, std::string date,
    std::function<void(std::vector<domain::ExerciseLog>)> onSuccess,
    std::function<void(std::string)>                      onError)
{
    auto self = shared_from_this();
    db_->execSqlAsync(
        "SELECT id, user_id, exercise_id, log_date::text, duration_minutes, calories_burned,"
        " sets, reps, distance_km, avg_heart_rate, notes, created_at::text"
        " FROM exercise_logs WHERE user_id = $1 AND log_date = $2 ORDER BY created_at",
        [self, onSuccess = std::move(onSuccess)](const drogon::orm::Result& r) {
            std::vector<domain::ExerciseLog> logs;
            logs.reserve(r.size());
            for (const auto& row : r) logs.push_back(rowToLog(row));
            onSuccess(std::move(logs));
        },
        [onError = std::move(onError)](const drogon::orm::DrogonDbException& e) {
            onError(e.base().what());
        },
        userId, std::move(date));
}

void PostgresExerciseLogRepository::create(
    domain::ExerciseLog log,
    std::function<void(domain::ExerciseLog)> onSuccess,
    std::function<void(std::string)>         onError)
{
    std::string setsSql     = log.sets.has_value()           ? std::to_string(*log.sets)           : "NULL";
    std::string repsSql     = log.reps.has_value()           ? std::to_string(*log.reps)           : "NULL";
    std::string distSql     = log.distance_km.has_value()    ? std::to_string(*log.distance_km)    : "NULL";
    std::string hrSql       = log.avg_heart_rate.has_value() ? std::to_string(*log.avg_heart_rate) : "NULL";
    std::string notesSql    = log.notes.empty() ? "NULL" : ("'" + log.notes + "'");

    std::string sql =
        "INSERT INTO exercise_logs"
        " (user_id, exercise_id, log_date, duration_minutes, calories_burned,"
        "  sets, reps, distance_km, avg_heart_rate, notes)"
        " VALUES ($1, $2, $3, $4, $5, " +
        setsSql + ", " + repsSql + ", " + distSql + ", " + hrSql + ", " + notesSql + ")"
        " RETURNING id, user_id, exercise_id, log_date::text, duration_minutes, calories_burned,"
        "  sets, reps, distance_km, avg_heart_rate, notes, created_at::text";

    auto self = shared_from_this();
    db_->execSqlAsync(
        sql,
        [self, onSuccess = std::move(onSuccess)](const drogon::orm::Result& r) {
            onSuccess(rowToLog(r[0]));
        },
        [onError = std::move(onError)](const drogon::orm::DrogonDbException& e) {
            onError(e.base().what());
        },
        log.user_id, log.exercise_id, log.log_date,
        log.duration_minutes, log.calories_burned);
}

void PostgresExerciseLogRepository::update(
    domain::ExerciseLog log,
    std::function<void(domain::ExerciseLog)> onSuccess,
    std::function<void(std::string)>         onError)
{
    std::string setsSql  = log.sets.has_value()           ? std::to_string(*log.sets)           : "NULL";
    std::string repsSql  = log.reps.has_value()           ? std::to_string(*log.reps)           : "NULL";
    std::string distSql  = log.distance_km.has_value()    ? std::to_string(*log.distance_km)    : "NULL";
    std::string hrSql    = log.avg_heart_rate.has_value() ? std::to_string(*log.avg_heart_rate) : "NULL";
    std::string notesSql = log.notes.empty() ? "NULL" : ("'" + log.notes + "'");

    std::string sql =
        "UPDATE exercise_logs SET"
        " exercise_id = $1, log_date = $2, duration_minutes = $3, calories_burned = $4,"
        " sets = " + setsSql + ", reps = " + repsSql + ", distance_km = " + distSql +
        ", avg_heart_rate = " + hrSql + ", notes = " + notesSql +
        " WHERE id = $5 AND user_id = $6"
        " RETURNING id, user_id, exercise_id, log_date::text, duration_minutes, calories_burned,"
        "  sets, reps, distance_km, avg_heart_rate, notes, created_at::text";

    auto self = shared_from_this();
    db_->execSqlAsync(
        sql,
        [self, onSuccess = std::move(onSuccess), onError]
        (const drogon::orm::Result& r) {
            if (r.empty()) { onError("Exercise log not found or access denied"); return; }
            onSuccess(rowToLog(r[0]));
        },
        [onError = std::move(onError)](const drogon::orm::DrogonDbException& e) {
            onError(e.base().what());
        },
        log.exercise_id, log.log_date, log.duration_minutes, log.calories_burned,
        log.id, log.user_id);
}

void PostgresExerciseLogRepository::remove(
    int64_t id, int64_t userId,
    std::function<void()>            onSuccess,
    std::function<void(std::string)> onError)
{
    auto self = shared_from_this();
    db_->execSqlAsync(
        "DELETE FROM exercise_logs WHERE id = $1 AND user_id = $2",
        [self, onSuccess = std::move(onSuccess)](const drogon::orm::Result&) {
            onSuccess();
        },
        [onError = std::move(onError)](const drogon::orm::DrogonDbException& e) {
            onError(e.base().what());
        },
        id, userId);
}

} // namespace infrastructure
