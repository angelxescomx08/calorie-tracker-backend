#include "infrastructure/persistence/PostgresSessionRepository.hpp"

namespace infrastructure {

PostgresSessionRepository::PostgresSessionRepository(drogon::orm::DbClientPtr db)
    : db_(std::move(db)) {}

domain::Session PostgresSessionRepository::rowToSession(const drogon::orm::Row& row)
{
    domain::Session s;
    s.id         = row["id"].as<int64_t>();
    s.user_id    = row["user_id"].as<int64_t>();
    s.token      = row["token"].as<std::string>();
    s.expires_at = row["expires_at"].as<std::string>();
    s.created_at = row["created_at"].as<std::string>();
    return s;
}

void PostgresSessionRepository::create(
    int64_t userId, std::string token, std::string expiresAt,
    std::function<void(domain::Session)>     onSuccess,
    std::function<void(std::string)>         onError)
{
    auto self = shared_from_this();
    db_->execSqlAsync(
        "INSERT INTO sessions (user_id, token, expires_at) VALUES ($1, $2, $3) "
        "RETURNING id, user_id, token, expires_at::text, created_at::text",
        [self, onSuccess = std::move(onSuccess)](const drogon::orm::Result& r) {
            onSuccess(rowToSession(r[0]));
        },
        [onError = std::move(onError)](const drogon::orm::DrogonDbException& e) {
            onError(e.base().what());
        },
        userId, std::move(token), std::move(expiresAt));
}

void PostgresSessionRepository::findByToken(
    std::string token,
    std::function<void(std::optional<domain::Session>)> onSuccess,
    std::function<void(std::string)>                    onError)
{
    auto self = shared_from_this();
    db_->execSqlAsync(
        "SELECT id, user_id, token, expires_at::text, created_at::text "
        "FROM sessions WHERE token = $1 AND expires_at > NOW()",
        [self, onSuccess = std::move(onSuccess)](const drogon::orm::Result& r) {
            if (r.empty()) { onSuccess(std::nullopt); return; }
            onSuccess(rowToSession(r[0]));
        },
        [onError = std::move(onError)](const drogon::orm::DrogonDbException& e) {
            onError(e.base().what());
        },
        std::move(token));
}

void PostgresSessionRepository::deleteByToken(
    std::string token,
    std::function<void()>            onSuccess,
    std::function<void(std::string)> onError)
{
    auto self = shared_from_this();
    db_->execSqlAsync(
        "DELETE FROM sessions WHERE token = $1",
        [self, onSuccess = std::move(onSuccess)](const drogon::orm::Result&) {
            onSuccess();
        },
        [onError = std::move(onError)](const drogon::orm::DrogonDbException& e) {
            onError(e.base().what());
        },
        std::move(token));
}

} // namespace infrastructure
