#include "infrastructure/persistence/PostgresWeightLogRepository.hpp"
#include <sstream>

namespace infrastructure {

PostgresWeightLogRepository::PostgresWeightLogRepository(drogon::orm::DbClientPtr db)
    : db_(std::move(db)) {}

domain::WeightLog PostgresWeightLogRepository::rowToLog(const drogon::orm::Row& row)
{
    domain::WeightLog w;
    w.id          = row["id"].as<int64_t>();
    w.user_id     = row["user_id"].as<int64_t>();
    w.weight_kg   = row["weight_kg"].as<double>();
    w.logged_date = row["logged_date"].as<std::string>();
    w.notes       = row["notes"].isNull() ? "" : row["notes"].as<std::string>();
    w.created_at  = row["created_at"].as<std::string>();
    return w;
}

void PostgresWeightLogRepository::listByUserId(
    int64_t userId, std::string startDate, std::string endDate, int limit,
    std::function<void(std::vector<domain::WeightLog>)> onSuccess,
    std::function<void(std::string)>                    onError)
{
    std::string sql =
        "SELECT id, user_id, weight_kg, logged_date::text, notes, created_at::text"
        " FROM weight_logs WHERE user_id = $1";
    if (!startDate.empty()) sql += " AND logged_date >= '" + startDate + "'";
    if (!endDate.empty())   sql += " AND logged_date <= '" + endDate   + "'";
    sql += " ORDER BY logged_date DESC LIMIT " + std::to_string(limit > 0 ? limit : 100);

    auto self = shared_from_this();
    db_->execSqlAsync(
        sql,
        [self, onSuccess = std::move(onSuccess)](const drogon::orm::Result& r) {
            std::vector<domain::WeightLog> logs;
            logs.reserve(r.size());
            for (const auto& row : r) logs.push_back(rowToLog(row));
            onSuccess(std::move(logs));
        },
        [onError = std::move(onError)](const drogon::orm::DrogonDbException& e) {
            onError(e.base().what());
        },
        userId);
}

void PostgresWeightLogRepository::findLatest(
    int64_t userId, std::string beforeDate,
    std::function<void(std::optional<domain::WeightLog>)> onSuccess,
    std::function<void(std::string)>                      onError)
{
    auto self = shared_from_this();
    db_->execSqlAsync(
        "SELECT id, user_id, weight_kg, logged_date::text, notes, created_at::text"
        " FROM weight_logs WHERE user_id = $1 AND logged_date <= $2"
        " ORDER BY logged_date DESC LIMIT 1",
        [self, onSuccess = std::move(onSuccess)](const drogon::orm::Result& r) {
            if (r.empty()) { onSuccess(std::nullopt); return; }
            onSuccess(rowToLog(r[0]));
        },
        [onError = std::move(onError)](const drogon::orm::DrogonDbException& e) {
            onError(e.base().what());
        },
        userId, std::move(beforeDate));
}

void PostgresWeightLogRepository::create(
    domain::WeightLog log,
    std::function<void(domain::WeightLog)>   onSuccess,
    std::function<void(std::string)>         onError)
{
    std::string notesSql = log.notes.empty() ? "NULL" : ("'" + log.notes + "'");
    std::string sql =
        "INSERT INTO weight_logs (user_id, weight_kg, logged_date, notes)"
        " VALUES ($1, $2, $3, " + notesSql + ")"
        " RETURNING id, user_id, weight_kg, logged_date::text, notes, created_at::text";

    auto self = shared_from_this();
    db_->execSqlAsync(
        sql,
        [self, onSuccess = std::move(onSuccess)](const drogon::orm::Result& r) {
            onSuccess(rowToLog(r[0]));
        },
        [onError = std::move(onError)](const drogon::orm::DrogonDbException& e) {
            onError(e.base().what());
        },
        log.user_id, log.weight_kg, log.logged_date);
}

void PostgresWeightLogRepository::update(
    domain::WeightLog log,
    std::function<void(domain::WeightLog)>   onSuccess,
    std::function<void(std::string)>         onError)
{
    std::string notesSql = log.notes.empty() ? "NULL" : ("'" + log.notes + "'");
    std::string sql =
        "UPDATE weight_logs SET weight_kg = $1, logged_date = $2, notes = " + notesSql + ""
        " WHERE id = $3 AND user_id = $4"
        " RETURNING id, user_id, weight_kg, logged_date::text, notes, created_at::text";

    auto self = shared_from_this();
    db_->execSqlAsync(
        sql,
        [self, onSuccess = std::move(onSuccess), onError]
        (const drogon::orm::Result& r) {
            if (r.empty()) { onError("Log not found or access denied"); return; }
            onSuccess(rowToLog(r[0]));
        },
        [onError = std::move(onError)](const drogon::orm::DrogonDbException& e) {
            onError(e.base().what());
        },
        log.weight_kg, log.logged_date, log.id, log.user_id);
}

void PostgresWeightLogRepository::remove(
    int64_t id, int64_t userId,
    std::function<void()>            onSuccess,
    std::function<void(std::string)> onError)
{
    auto self = shared_from_this();
    db_->execSqlAsync(
        "DELETE FROM weight_logs WHERE id = $1 AND user_id = $2",
        [self, onSuccess = std::move(onSuccess)](const drogon::orm::Result&) {
            onSuccess();
        },
        [onError = std::move(onError)](const drogon::orm::DrogonDbException& e) {
            onError(e.base().what());
        },
        id, userId);
}

} // namespace infrastructure
