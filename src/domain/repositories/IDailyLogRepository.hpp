#pragma once
#include <functional>
#include <optional>
#include <string>
#include "domain/entities/DailyLog.hpp"

namespace domain {

class IDailyLogRepository {
public:
    virtual ~IDailyLogRepository() = default;

    virtual void findByUserAndDate(
        int64_t     userId,
        std::string date,
        std::function<void(std::optional<DailyLog>)> onSuccess,
        std::function<void(std::string)>             onError) = 0;

    // Returns existing log or creates a new one.
    virtual void getOrCreate(
        int64_t     userId,
        std::string date,
        std::function<void(DailyLog)>    onSuccess,
        std::function<void(std::string)> onError) = 0;

    virtual void update(
        DailyLog log,
        std::function<void(DailyLog)>    onSuccess,
        std::function<void(std::string)> onError) = 0;
};

} // namespace domain
