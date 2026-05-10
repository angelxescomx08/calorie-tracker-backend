#pragma once
#include <memory>
#include <drogon/orm/DbClient.h>
#include "domain/repositories/IUserGoalRepository.hpp"

namespace infrastructure {

class PostgresUserGoalRepository
    : public domain::IUserGoalRepository
    , public std::enable_shared_from_this<PostgresUserGoalRepository>
{
public:
    explicit PostgresUserGoalRepository(drogon::orm::DbClientPtr db);

    void findActiveByUserId(int64_t userId,
        std::function<void(std::optional<domain::UserGoal>)> onSuccess,
        std::function<void(std::string)>                     onError) override;

    void create(domain::UserGoal goal,
        std::function<void(domain::UserGoal)>    onSuccess,
        std::function<void(std::string)>         onError) override;

    void update(domain::UserGoal goal,
        std::function<void(domain::UserGoal)>    onSuccess,
        std::function<void(std::string)>         onError) override;

    void deactivate(int64_t id, int64_t userId,
        std::function<void()>            onSuccess,
        std::function<void(std::string)> onError) override;

private:
    drogon::orm::DbClientPtr db_;
    static domain::UserGoal rowToGoal(const drogon::orm::Row& row);
};

} // namespace infrastructure
