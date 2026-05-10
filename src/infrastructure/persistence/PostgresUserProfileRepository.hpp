#pragma once
#include <memory>
#include <drogon/orm/DbClient.h>
#include "domain/repositories/IUserProfileRepository.hpp"

namespace infrastructure {

class PostgresUserProfileRepository
    : public domain::IUserProfileRepository
    , public std::enable_shared_from_this<PostgresUserProfileRepository>
{
public:
    explicit PostgresUserProfileRepository(drogon::orm::DbClientPtr db);

    void findByUserId(int64_t userId,
        std::function<void(std::optional<domain::UserProfile>)> onSuccess,
        std::function<void(std::string)>                        onError) override;

    void upsert(domain::UserProfile profile,
        std::function<void(domain::UserProfile)> onSuccess,
        std::function<void(std::string)>         onError) override;

private:
    drogon::orm::DbClientPtr db_;
    static domain::UserProfile rowToProfile(const drogon::orm::Row& row);
};

} // namespace infrastructure
