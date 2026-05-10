#pragma once
#include <memory>
#include <functional>
#include <string>
#include "domain/repositories/IUserRepository.hpp"

namespace application {

class GetCurrentUserUseCase : public std::enable_shared_from_this<GetCurrentUserUseCase> {
    std::shared_ptr<domain::IUserRepository> userRepo_;
public:
    explicit GetCurrentUserUseCase(std::shared_ptr<domain::IUserRepository> userRepo)
        : userRepo_(std::move(userRepo)) {}

    void execute(
        int64_t userId,
        std::function<void(domain::User)>    onSuccess,
        std::function<void(std::string)>     onError)
    {
        auto self         = shared_from_this();
        auto onErrorPtr   = std::make_shared<decltype(onError)>(std::move(onError));

        userRepo_->findById(userId,
            [onSuccess = std::move(onSuccess), onErrorPtr](std::optional<domain::User> u) {
                if (!u) { (*onErrorPtr)("User not found"); return; }
                onSuccess(*u);
            },
            [onErrorPtr](std::string err) { (*onErrorPtr)(std::move(err)); });
    }
};

} // namespace application
