#pragma once
#include <memory>
#include <functional>
#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <ctime>
#include "domain/repositories/IUserRepository.hpp"
#include "domain/repositories/ISessionRepository.hpp"
#include "application/ports/IPasswordHasher.hpp"
#include "application/ports/ITokenGenerator.hpp"

namespace application {

class RegisterUseCase : public std::enable_shared_from_this<RegisterUseCase> {
    std::shared_ptr<domain::IUserRepository>    userRepo_;
    std::shared_ptr<domain::ISessionRepository> sessionRepo_;
    std::shared_ptr<IPasswordHasher>            hasher_;
    std::shared_ptr<ITokenGenerator>            tokenGen_;

    static std::string expiresIn14Days() {
        auto now     = std::chrono::system_clock::now();
        auto expires = now + std::chrono::hours(14 * 24);
        auto tt      = std::chrono::system_clock::to_time_t(expires);
        std::tm tm{};
#ifdef _WIN32
        gmtime_s(&tm, &tt);
#else
        gmtime_r(&tt, &tm);
#endif
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
        return oss.str();
    }

public:
    RegisterUseCase(
        std::shared_ptr<domain::IUserRepository>    userRepo,
        std::shared_ptr<domain::ISessionRepository> sessionRepo,
        std::shared_ptr<IPasswordHasher>            hasher,
        std::shared_ptr<ITokenGenerator>            tokenGen)
        : userRepo_(std::move(userRepo))
        , sessionRepo_(std::move(sessionRepo))
        , hasher_(std::move(hasher))
        , tokenGen_(std::move(tokenGen))
    {}

    void execute(
        const std::string& name,
        const std::string& email,
        const std::string& password,
        std::function<void(domain::User, domain::Session)> onSuccess,
        std::function<void(std::string)>                   onError)
    {
        auto self         = shared_from_this();
        auto onSuccessPtr = std::make_shared<decltype(onSuccess)>(std::move(onSuccess));
        auto onErrorPtr   = std::make_shared<decltype(onError)>(std::move(onError));

        userRepo_->findByEmail(email,
            [self, name, email, password, onSuccessPtr, onErrorPtr]
            (std::optional<domain::User> existing) {
                if (existing) { (*onErrorPtr)("Email already registered"); return; }

                std::string hash = self->hasher_->hash(password);
                self->userRepo_->createWithPassword(name, email, hash,
                    [self, onSuccessPtr, onErrorPtr](domain::User user) {
                        std::string token   = self->tokenGen_->generate();
                        std::string expires = expiresIn14Days();
                        self->sessionRepo_->create(user.id, token, expires,
                            [onSuccessPtr, user](domain::Session session) {
                                (*onSuccessPtr)(user, session);
                            },
                            [onErrorPtr](std::string err) { (*onErrorPtr)(std::move(err)); });
                    },
                    [onErrorPtr](std::string err) { (*onErrorPtr)(std::move(err)); });
            },
            [onErrorPtr](std::string err) { (*onErrorPtr)(std::move(err)); });
    }
};

} // namespace application
