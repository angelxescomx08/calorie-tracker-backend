#pragma once
#include <memory>
#include <drogon/orm/DbClient.h>
#include "domain/repositories/ISessionRepository.hpp"

namespace infrastructure {

class PostgresSessionRepository
    : public domain::ISessionRepository
    , public std::enable_shared_from_this<PostgresSessionRepository>
{
public:
    explicit PostgresSessionRepository(drogon::orm::DbClientPtr db);

    void create(int64_t userId, std::string token, std::string expiresAt,
        std::function<void(domain::Session)>     onSuccess,
        std::function<void(std::string)>         onError) override;

    void findByToken(std::string token,
        std::function<void(std::optional<domain::Session>)> onSuccess,
        std::function<void(std::string)>                    onError) override;

    void deleteByToken(std::string token,
        std::function<void()>            onSuccess,
        std::function<void(std::string)> onError) override;

private:
    drogon::orm::DbClientPtr db_;
    static domain::Session rowToSession(const drogon::orm::Row& row);
};

} // namespace infrastructure
