#pragma once
#include <functional>
#include <vector>
#include <string>
#include "domain/entities/ExerciseLog.hpp"

namespace domain {

class IExerciseLogRepository {
public:
    virtual ~IExerciseLogRepository() = default;

    virtual void listByUserAndDate(
        int64_t     userId,
        std::string date,
        std::function<void(std::vector<ExerciseLog>)> onSuccess,
        std::function<void(std::string)>              onError) = 0;

    virtual void create(
        ExerciseLog log,
        std::function<void(ExerciseLog)> onSuccess,
        std::function<void(std::string)> onError) = 0;

    virtual void update(
        ExerciseLog log,
        std::function<void(ExerciseLog)> onSuccess,
        std::function<void(std::string)> onError) = 0;

    virtual void remove(
        int64_t id,
        int64_t userId,
        std::function<void()>            onSuccess,
        std::function<void(std::string)> onError) = 0;
};

} // namespace domain
