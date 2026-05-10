#pragma once
#include <functional>
#include <optional>
#include <vector>
#include <string>
#include "domain/entities/MealEntry.hpp"

namespace domain {

class IMealEntryRepository {
public:
    virtual ~IMealEntryRepository() = default;

    virtual void listByDailyLog(
        int64_t dailyLogId,
        std::function<void(std::vector<MealEntry>)> onSuccess,
        std::function<void(std::string)>            onError) = 0;

    virtual void create(
        MealEntry entry,
        std::function<void(MealEntry)>   onSuccess,
        std::function<void(std::string)> onError) = 0;

    virtual void update(
        MealEntry entry,
        std::function<void(MealEntry)>   onSuccess,
        std::function<void(std::string)> onError) = 0;

    virtual void remove(
        int64_t id,
        int64_t dailyLogId,
        std::function<void()>            onSuccess,
        std::function<void(std::string)> onError) = 0;
};

} // namespace domain
