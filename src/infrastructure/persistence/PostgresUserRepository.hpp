#pragma once
#include <memory>
#include <drogon/orm/DbClient.h>
#include "domain/repositories/IUserRepository.hpp"

namespace infrastructure {

class PostgresUserRepository
    : public domain::IUserRepository
    , public std::enable_shared_from_this<PostgresUserRepository>
{
public:
    explicit PostgresUserRepository(drogon::orm::DbClientPtr db);

    void findById(int64_t id,
        std::function<void(std::optional<domain::User>)> onSuccess,
        std::function<void(std::string)>                 onError) override;

    void findByEmail(std::string email,
        std::function<void(std::optional<domain::User>)> onSuccess,
        std::function<void(std::string)>                 onError) override;

    void createWithPassword(std::string name, std::string email, std::string passwordHash,
        std::function<void(domain::User)>    onSuccess,
        std::function<void(std::string)>     onError) override;

    void getPasswordHash(int64_t userId,
        std::function<void(std::string)> onSuccess,
        std::function<void(std::string)> onError) override;

private:
    drogon::orm::DbClientPtr db_;
    static domain::User rowToUser(const drogon::orm::Row& row);
};

} // namespace infrastructure
