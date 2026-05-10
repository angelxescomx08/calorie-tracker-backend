#pragma once
#include <functional>
#include <optional>
#include <vector>
#include <string>
#include "domain/entities/WeightLog.hpp"

namespace domain {

class IWeightLogRepository {
public:
    virtual ~IWeightLogRepository() = default;

    virtual void listByUserId(
        int64_t     userId,
        std::string startDate, // YYYY-MM-DD, empty = no lower bound
        std::string endDate,   // YYYY-MM-DD, empty = no upper bound
        int         limit,
        std::function<void(std::vector<WeightLog>)> onSuccess,
        std::function<void(std::string)>            onError) = 0;

    // Returns the most recent log on or before beforeDate.
    virtual void findLatest(
        int64_t     userId,
        std::string beforeDate, // inclusive
        std::function<void(std::optional<WeightLog>)> onSuccess,
        std::function<void(std::string)>              onError) = 0;

    virtual void create(
        WeightLog log,
        std::function<void(WeightLog)>   onSuccess,
        std::function<void(std::string)> onError) = 0;

    virtual void update(
        WeightLog log,
        std::function<void(WeightLog)>   onSuccess,
        std::function<void(std::string)> onError) = 0;

    // Verifies ownership before deleting.
    virtual void remove(
        int64_t id,
        int64_t userId,
        std::function<void()>            onSuccess,
        std::function<void(std::string)> onError) = 0;
};

} // namespace domain
