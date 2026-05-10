#include "infrastructure/persistence/PostgresUserRepository.hpp"

namespace infrastructure {

PostgresUserRepository::PostgresUserRepository(drogon::orm::DbClientPtr db)
    : db_(std::move(db)) {}

domain::User PostgresUserRepository::rowToUser(const drogon::orm::Row& row)
{
    domain::User u;
    u.id              = row["id"].as<int64_t>();
    u.email           = row["email"].as<std::string>();
    u.name            = row["name"].as<std::string>();
    u.profile_picture = row["profile_picture"].isNull() ? "" : row["profile_picture"].as<std::string>();
    u.email_verified  = row["email_verified"].as<bool>();
    u.created_at      = row["created_at"].as<std::string>();
    u.updated_at      = row["updated_at"].as<std::string>();
    return u;
}

void PostgresUserRepository::findById(
    int64_t id,
    std::function<void(std::optional<domain::User>)> onSuccess,
    std::function<void(std::string)>                 onError)
{
    auto self = shared_from_this();
    db_->execSqlAsync(
        "SELECT id, email, name, profile_picture, email_verified, "
        "created_at::text, updated_at::text FROM users WHERE id = $1",
        [self, onSuccess = std::move(onSuccess)](const drogon::orm::Result& r) {
            if (r.empty()) { onSuccess(std::nullopt); return; }
            onSuccess(rowToUser(r[0]));
        },
        [onError = std::move(onError)](const drogon::orm::DrogonDbException& e) {
            onError(e.base().what());
        },
        id);
}

void PostgresUserRepository::findByEmail(
    std::string email,
    std::function<void(std::optional<domain::User>)> onSuccess,
    std::function<void(std::string)>                 onError)
{
    auto self = shared_from_this();
    db_->execSqlAsync(
        "SELECT id, email, name, profile_picture, email_verified, "
        "created_at::text, updated_at::text FROM users WHERE email = $1",
        [self, onSuccess = std::move(onSuccess)](const drogon::orm::Result& r) {
            if (r.empty()) { onSuccess(std::nullopt); return; }
            onSuccess(rowToUser(r[0]));
        },
        [onError = std::move(onError)](const drogon::orm::DrogonDbException& e) {
            onError(e.base().what());
        },
        std::move(email));
}

void PostgresUserRepository::createWithPassword(
    std::string name,
    std::string email,
    std::string passwordHash,
    std::function<void(domain::User)>    onSuccess,
    std::function<void(std::string)>     onError)
{
    // Atomic CTE: insert user + auth_provider in one statement.
    static const char* kSql =
        "WITH new_user AS ("
        "  INSERT INTO users (email, name)"
        "  VALUES ($1, $2)"
        "  RETURNING id, email, name, profile_picture, email_verified,"
        "            created_at::text, updated_at::text"
        "), auth AS ("
        "  INSERT INTO auth_providers (user_id, provider, password_hash)"
        "  SELECT id, 'email', $3 FROM new_user"
        ")"
        "SELECT * FROM new_user";

    auto self = shared_from_this();
    db_->execSqlAsync(
        kSql,
        [self, onSuccess = std::move(onSuccess)](const drogon::orm::Result& r) {
            onSuccess(rowToUser(r[0]));
        },
        [onError = std::move(onError)](const drogon::orm::DrogonDbException& e) {
            onError(e.base().what());
        },
        email, name, passwordHash);
}

void PostgresUserRepository::getPasswordHash(
    int64_t userId,
    std::function<void(std::string)> onSuccess,
    std::function<void(std::string)> onError)
{
    auto self = shared_from_this();
    db_->execSqlAsync(
        "SELECT password_hash FROM auth_providers "
        "WHERE user_id = $1 AND provider = 'email'",
        [self, onSuccess = std::move(onSuccess), onError]
        (const drogon::orm::Result& r) {
            if (r.empty() || r[0]["password_hash"].isNull()) {
                onError("No password set for this user");
                return;
            }
            onSuccess(r[0]["password_hash"].as<std::string>());
        },
        [onError = std::move(onError)](const drogon::orm::DrogonDbException& e) {
            onError(e.base().what());
        },
        userId);
}

} // namespace infrastructure
