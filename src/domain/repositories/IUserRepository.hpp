#pragma once
#include <functional>
#include <optional>
#include <string>
#include "domain/entities/User.hpp"

namespace domain {

class IUserRepository {
public:
    virtual ~IUserRepository() = default;

    virtual void findById(
        int64_t id,
        std::function<void(std::optional<User>)> onSuccess,
        std::function<void(std::string)>         onError) = 0;

    virtual void findByEmail(
        std::string email,
        std::function<void(std::optional<User>)> onSuccess,
        std::function<void(std::string)>         onError) = 0;

    // Creates user + email auth_provider row atomically.
    // passwordHash is the PBKDF2 hash to store.
    virtual void createWithPassword(
        std::string name,
        std::string email,
        std::string passwordHash,
        std::function<void(User)>       onSuccess,
        std::function<void(std::string)> onError) = 0;

    // Returns the stored password hash for the user (from auth_providers).
    virtual void getPasswordHash(
        int64_t userId,
        std::function<void(std::string)> onSuccess,
        std::function<void(std::string)> onError) = 0;
};

} // namespace domain
