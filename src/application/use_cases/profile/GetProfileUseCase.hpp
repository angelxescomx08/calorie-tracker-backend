#pragma once
#include <memory>
#include <functional>
#include <string>
#include "domain/repositories/IUserProfileRepository.hpp"

namespace application {

class GetProfileUseCase : public std::enable_shared_from_this<GetProfileUseCase> {
    std::shared_ptr<domain::IUserProfileRepository> repo_;
public:
    explicit GetProfileUseCase(std::shared_ptr<domain::IUserProfileRepository> repo)
        : repo_(std::move(repo)) {}

    void execute(
        int64_t userId,
        std::function<void(std::optional<domain::UserProfile>)> onSuccess,
        std::function<void(std::string)>                        onError)
    {
        repo_->findByUserId(userId, std::move(onSuccess), std::move(onError));
    }
};

} // namespace application
