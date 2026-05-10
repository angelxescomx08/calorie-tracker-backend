#pragma once
#include <memory>
#include <drogon/orm/DbClient.h>
#include "domain/repositories/IDailyLogRepository.hpp"

namespace infrastructure {

class PostgresDailyLogRepository
    : public domain::IDailyLogRepository
    , public std::enable_shared_from_this<PostgresDailyLogRepository>
{
public:
    explicit PostgresDailyLogRepository(drogon::orm::DbClientPtr db);

    void findByUserAndDate(int64_t userId, std::string date,
        std::function<void(std::optional<domain::DailyLog>)> onSuccess,
        std::function<void(std::string)>                     onError) override;

    void getOrCreate(int64_t userId, std::string date,
        std::function<void(domain::DailyLog)>    onSuccess,
        std::function<void(std::string)>         onError) override;

    void update(domain::DailyLog log,
        std::function<void(domain::DailyLog)>    onSuccess,
        std::function<void(std::string)>         onError) override;

private:
    drogon::orm::DbClientPtr db_;
    static domain::DailyLog rowToLog(const drogon::orm::Row& row);
};

} // namespace infrastructure
