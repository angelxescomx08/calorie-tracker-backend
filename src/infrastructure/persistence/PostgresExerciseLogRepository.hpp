#pragma once
#include <memory>
#include <drogon/orm/DbClient.h>
#include "domain/repositories/IExerciseLogRepository.hpp"

namespace infrastructure {

class PostgresExerciseLogRepository
    : public domain::IExerciseLogRepository
    , public std::enable_shared_from_this<PostgresExerciseLogRepository>
{
public:
    explicit PostgresExerciseLogRepository(drogon::orm::DbClientPtr db);

    void listByUserAndDate(int64_t userId, std::string date,
        std::function<void(std::vector<domain::ExerciseLog>)> onSuccess,
        std::function<void(std::string)>                      onError) override;

    void create(domain::ExerciseLog log,
        std::function<void(domain::ExerciseLog)> onSuccess,
        std::function<void(std::string)>         onError) override;

    void update(domain::ExerciseLog log,
        std::function<void(domain::ExerciseLog)> onSuccess,
        std::function<void(std::string)>         onError) override;

    void remove(int64_t id, int64_t userId,
        std::function<void()>            onSuccess,
        std::function<void(std::string)> onError) override;

private:
    drogon::orm::DbClientPtr db_;
    static domain::ExerciseLog rowToLog(const drogon::orm::Row& row);
};

} // namespace infrastructure
