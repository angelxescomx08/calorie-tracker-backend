#pragma once
#include <functional>
#include <optional>
#include <string>
#include <cstdint>
#include "domain/entities/Session.hpp"

namespace domain {

class ISessionRepository {
public:
    virtual ~ISessionRepository() = default;

    virtual void create(
        int64_t     userId,
        std::string token,
        std::string expiresAt,
        std::function<void(Session)>     onSuccess,
        std::function<void(std::string)> onError) = 0;

    virtual void findByToken(
        std::string token,
        std::function<void(std::optional<Session>)> onSuccess,
        std::function<void(std::string)>             onError) = 0;

    virtual void deleteByToken(
        std::string token,
        std::function<void()>            onSuccess,
        std::function<void(std::string)> onError) = 0;
};

} // namespace domain
