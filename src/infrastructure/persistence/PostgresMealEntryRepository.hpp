#pragma once
#include <memory>
#include <drogon/orm/DbClient.h>
#include "domain/repositories/IMealEntryRepository.hpp"

namespace infrastructure {

class PostgresMealEntryRepository
    : public domain::IMealEntryRepository
    , public std::enable_shared_from_this<PostgresMealEntryRepository>
{
public:
    explicit PostgresMealEntryRepository(drogon::orm::DbClientPtr db);

    void listByDailyLog(int64_t dailyLogId,
        std::function<void(std::vector<domain::MealEntry>)> onSuccess,
        std::function<void(std::string)>                    onError) override;

    void create(domain::MealEntry entry,
        std::function<void(domain::MealEntry)>   onSuccess,
        std::function<void(std::string)>         onError) override;

    void update(domain::MealEntry entry,
        std::function<void(domain::MealEntry)>   onSuccess,
        std::function<void(std::string)>         onError) override;

    void remove(int64_t id, int64_t dailyLogId,
        std::function<void()>            onSuccess,
        std::function<void(std::string)> onError) override;

private:
    drogon::orm::DbClientPtr db_;
    static domain::MealEntry rowToEntry(const drogon::orm::Row& row);
};

} // namespace infrastructure
