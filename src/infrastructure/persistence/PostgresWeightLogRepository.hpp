#pragma once
#include <memory>
#include <drogon/orm/DbClient.h>
#include "domain/repositories/IWeightLogRepository.hpp"

namespace infrastructure {

class PostgresWeightLogRepository
    : public domain::IWeightLogRepository
    , public std::enable_shared_from_this<PostgresWeightLogRepository>
{
public:
    explicit PostgresWeightLogRepository(drogon::orm::DbClientPtr db);

    void listByUserId(int64_t userId,
        std::string startDate, std::string endDate, int limit,
        std::function<void(std::vector<domain::WeightLog>)> onSuccess,
        std::function<void(std::string)>                    onError) override;

    void findLatest(int64_t userId, std::string beforeDate,
        std::function<void(std::optional<domain::WeightLog>)> onSuccess,
        std::function<void(std::string)>                      onError) override;

    void create(domain::WeightLog log,
        std::function<void(domain::WeightLog)>   onSuccess,
        std::function<void(std::string)>         onError) override;

    void update(domain::WeightLog log,
        std::function<void(domain::WeightLog)>   onSuccess,
        std::function<void(std::string)>         onError) override;

    void remove(int64_t id, int64_t userId,
        std::function<void()>            onSuccess,
        std::function<void(std::string)> onError) override;

private:
    drogon::orm::DbClientPtr db_;
    static domain::WeightLog rowToLog(const drogon::orm::Row& row);
};

} // namespace infrastructure
