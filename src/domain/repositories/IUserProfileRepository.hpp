#pragma once
#include <functional>
#include <optional>
#include <string>
#include "domain/entities/UserProfile.hpp"

namespace domain {

class IUserProfileRepository {
public:
    virtual ~IUserProfileRepository() = default;

    virtual void findByUserId(
        int64_t userId,
        std::function<void(std::optional<UserProfile>)> onSuccess,
        std::function<void(std::string)>                onError) = 0;

    // INSERT on first call, UPDATE on subsequent calls.
    virtual void upsert(
        UserProfile profile,
        std::function<void(UserProfile)> onSuccess,
        std::function<void(std::string)> onError) = 0;
};

} // namespace domain
