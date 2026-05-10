#include "infrastructure/persistence/PostgresBodyMeasurementRepository.hpp"
#include <cmath>
#include <sstream>

namespace infrastructure {

PostgresBodyMeasurementRepository::PostgresBodyMeasurementRepository(drogon::orm::DbClientPtr db)
    : db_(std::move(db)) {}

domain::BodyMeasurement PostgresBodyMeasurementRepository::rowToMeasurement(
    const drogon::orm::Row& row)
{
    domain::BodyMeasurement m;
    m.id            = row["id"].as<int64_t>();
    m.user_id       = row["user_id"].as<int64_t>();
    m.measured_date = row["measured_date"].as<std::string>();
    m.neck_cm       = row["neck_cm"].as<double>();
    m.waist_cm      = row["waist_cm"].as<double>();
    m.hip_cm        = row["hip_cm"].isNull()
                        ? std::optional<double>(std::nullopt)
                        : std::optional<double>(row["hip_cm"].as<double>());
    m.body_fat_percentage = row["body_fat_percentage"].isNull()
                            ? std::optional<double>(std::nullopt)
                            : std::optional<double>(row["body_fat_percentage"].as<double>());
    m.notes         = row["notes"].isNull() ? "" : row["notes"].as<std::string>();
    m.created_at    = row["created_at"].as<std::string>();
    return m;
}

void PostgresBodyMeasurementRepository::listByUserId(
    int64_t userId, std::string startDate, std::string endDate,
    std::function<void(std::vector<domain::BodyMeasurement>)> onSuccess,
    std::function<void(std::string)>                          onError)
{
    std::string sql =
        "SELECT id, user_id, measured_date::text, neck_cm, waist_cm, hip_cm,"
        " body_fat_percentage, notes, created_at::text"
        " FROM body_measurements WHERE user_id = $1";
    if (!startDate.empty()) sql += " AND measured_date >= '" + startDate + "'";
    if (!endDate.empty())   sql += " AND measured_date <= '" + endDate   + "'";
    sql += " ORDER BY measured_date DESC";

    auto self = shared_from_this();
    db_->execSqlAsync(
        sql,
        [self, onSuccess = std::move(onSuccess)](const drogon::orm::Result& r) {
            std::vector<domain::BodyMeasurement> ms;
            ms.reserve(r.size());
            for (const auto& row : r) ms.push_back(rowToMeasurement(row));
            onSuccess(std::move(ms));
        },
        [onError = std::move(onError)](const drogon::orm::DrogonDbException& e) {
            onError(e.base().what());
        },
        userId);
}

void PostgresBodyMeasurementRepository::create(
    domain::BodyMeasurement m,
    std::function<void(domain::BodyMeasurement)> onSuccess,
    std::function<void(std::string)>             onError)
{
    // US Navy body fat formula (stored as computed value).
    auto computeBF = [&]() -> std::string {
        if (m.hip_cm.has_value()) {
            // Female formula
            double bf = 163.205 * std::log10(m.waist_cm + *m.hip_cm - m.neck_cm)
                      - 97.684  * std::log10(0.0) // placeholder, use height from profile
                      - 78.387;
            // We don't have height here; store NULL and let use case compute it.
            return "NULL";
        }
        // Male formula: needs height — store NULL, use case should pre-compute and pass.
        return "NULL";
    };

    // body_fat_percentage must be pre-computed by the use case.
    std::string bfSql = m.body_fat_percentage.has_value()
        ? std::to_string(*m.body_fat_percentage)
        : "NULL";
    std::string hipSql = m.hip_cm.has_value()
        ? std::to_string(*m.hip_cm)
        : "NULL";
    std::string notesSql = m.notes.empty() ? "NULL" : ("'" + m.notes + "'");

    std::string sql =
        "INSERT INTO body_measurements"
        " (user_id, measured_date, neck_cm, waist_cm, hip_cm, body_fat_percentage, notes)"
        " VALUES ($1, $2, $3, $4, " + hipSql + ", " + bfSql + ", " + notesSql + ")"
        " RETURNING id, user_id, measured_date::text, neck_cm, waist_cm, hip_cm,"
        " body_fat_percentage, notes, created_at::text";

    auto self = shared_from_this();
    db_->execSqlAsync(
        sql,
        [self, onSuccess = std::move(onSuccess)](const drogon::orm::Result& r) {
            onSuccess(rowToMeasurement(r[0]));
        },
        [onError = std::move(onError)](const drogon::orm::DrogonDbException& e) {
            onError(e.base().what());
        },
        m.user_id, m.measured_date, m.neck_cm, m.waist_cm);
}

void PostgresBodyMeasurementRepository::remove(
    int64_t id, int64_t userId,
    std::function<void()>            onSuccess,
    std::function<void(std::string)> onError)
{
    auto self = shared_from_this();
    db_->execSqlAsync(
        "DELETE FROM body_measurements WHERE id = $1 AND user_id = $2",
        [self, onSuccess = std::move(onSuccess)](const drogon::orm::Result&) {
            onSuccess();
        },
        [onError = std::move(onError)](const drogon::orm::DrogonDbException& e) {
            onError(e.base().what());
        },
        id, userId);
}

} // namespace infrastructure
