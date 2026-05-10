#pragma once
#include <memory>
#include <functional>
#include <string>
#include "domain/repositories/IUserProfileRepository.hpp"

namespace application {

class UpsertProfileUseCase : public std::enable_shared_from_this<UpsertProfileUseCase> {
    std::shared_ptr<domain::IUserProfileRepository> repo_;
public:
    explicit UpsertProfileUseCase(std::shared_ptr<domain::IUserProfileRepository> repo)
        : repo_(std::move(repo)) {}

    void execute(
        domain::UserProfile profile,
        std::function<void(domain::UserProfile)> onSuccess,
        std::function<void(std::string)>         onError)
    {
        repo_->upsert(std::move(profile), std::move(onSuccess), std::move(onError));
    }
};

} // namespace application
