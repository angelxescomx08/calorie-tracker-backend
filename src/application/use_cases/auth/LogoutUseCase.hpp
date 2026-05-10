#pragma once
#include <memory>
#include <functional>
#include <string>
#include "domain/repositories/ISessionRepository.hpp"

namespace application {

class LogoutUseCase : public std::enable_shared_from_this<LogoutUseCase> {
    std::shared_ptr<domain::ISessionRepository> sessionRepo_;
public:
    explicit LogoutUseCase(std::shared_ptr<domain::ISessionRepository> sessionRepo)
        : sessionRepo_(std::move(sessionRepo)) {}

    void execute(
        const std::string& token,
        std::function<void()>            onSuccess,
        std::function<void(std::string)> onError)
    {
        sessionRepo_->deleteByToken(token,
            std::move(onSuccess),
            std::move(onError));
    }
};

} // namespace application
