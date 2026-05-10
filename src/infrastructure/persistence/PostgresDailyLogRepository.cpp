#include "infrastructure/persistence/PostgresDailyLogRepository.hpp"

namespace infrastructure {

PostgresDailyLogRepository::PostgresDailyLogRepository(drogon::orm::DbClientPtr db)
    : db_(std::move(db)) {}

domain::DailyLog PostgresDailyLogRepository::rowToLog(const drogon::orm::Row& row)
{
    domain::DailyLog d;
    d.id         = row["id"].as<int64_t>();
    d.user_id    = row["user_id"].as<int64_t>();
    d.log_date   = row["log_date"].as<std::string>();
    d.water_ml   = row["water_ml"].as<int>();
    d.notes      = row["notes"].isNull() ? "" : row["notes"].as<std::string>();
    d.created_at = row["created_at"].as<std::string>();
    d.updated_at = row["updated_at"].as<std::string>();
    return d;
}

void PostgresDailyLogRepository::findByUserAndDate(
    int64_t userId, std::string date,
    std::function<void(std::optional<domain::DailyLog>)> onSuccess,
    std::function<void(std::string)>                     onError)
{
    auto self = shared_from_this();
    db_->execSqlAsync(
        "SELECT id, user_id, log_date::text, water_ml, notes, created_at::text, updated_at::text"
        " FROM daily_logs WHERE user_id = $1 AND log_date = $2",
        [self, onSuccess = std::move(onSuccess)](const drogon::orm::Result& r) {
            if (r.empty()) { onSuccess(std::nullopt); return; }
            onSuccess(rowToLog(r[0]));
        },
        [onError = std::move(onError)](const drogon::orm::DrogonDbException& e) {
            onError(e.base().what());
        },
        userId, std::move(date));
}

void PostgresDailyLogRepository::getOrCreate(
    int64_t userId, std::string date,
    std::function<void(domain::DailyLog)>    onSuccess,
    std::function<void(std::string)>         onError)
{
    auto self = shared_from_this();
    db_->execSqlAsync(
        "INSERT INTO daily_logs (user_id, log_date)"
        " VALUES ($1, $2)"
        " ON CONFLICT (user_id, log_date) DO UPDATE SET updated_at = daily_logs.updated_at"
        " RETURNING id, user_id, log_date::text, water_ml, notes, created_at::text, updated_at::text",
        [self, onSuccess = std::move(onSuccess)](const drogon::orm::Result& r) {
            onSuccess(rowToLog(r[0]));
        },
        [onError = std::move(onError)](const drogon::orm::DrogonDbException& e) {
            onError(e.base().what());
        },
        userId, std::move(date));
}

void PostgresDailyLogRepository::update(
    domain::DailyLog log,
    std::function<void(domain::DailyLog)>    onSuccess,
    std::function<void(std::string)>         onError)
{
    std::string notesSql = log.notes.empty() ? "NULL" : ("'" + log.notes + "'");
    std::string sql =
        "UPDATE daily_logs SET water_ml = $1, notes = " + notesSql + ", updated_at = NOW()"
        " WHERE id = $2 AND user_id = $3"
        " RETURNING id, user_id, log_date::text, water_ml, notes, created_at::text, updated_at::text";

    auto self = shared_from_this();
    db_->execSqlAsync(
        sql,
        [self, onSuccess = std::move(onSuccess), onError]
        (const drogon::orm::Result& r) {
            if (r.empty()) { onError("Daily log not found"); return; }
            onSuccess(rowToLog(r[0]));
        },
        [onError = std::move(onError)](const drogon::orm::DrogonDbException& e) {
            onError(e.base().what());
        },
        log.water_ml, log.id, log.user_id);
}

} // namespace infrastructure
