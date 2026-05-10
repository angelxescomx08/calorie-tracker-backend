#pragma once
#include <functional>
#include <optional>
#include <vector>
#include <string>
#include "domain/entities/UserGoal.hpp"

namespace domain {

class IUserGoalRepository {
public:
    virtual ~IUserGoalRepository() = default;

    virtual void findActiveByUserId(
        int64_t userId,
        std::function<void(std::optional<UserGoal>)> onSuccess,
        std::function<void(std::string)>             onError) = 0;

    virtual void create(
        UserGoal goal,
        std::function<void(UserGoal)>    onSuccess,
        std::function<void(std::string)> onError) = 0;

    virtual void update(
        UserGoal goal,
        std::function<void(UserGoal)>    onSuccess,
        std::function<void(std::string)> onError) = 0;

    // Sets is_active = false. Verifies ownership.
    virtual void deactivate(
        int64_t id,
        int64_t userId,
        std::function<void()>            onSuccess,
        std::function<void(std::string)> onError) = 0;
};

} // namespace domain
