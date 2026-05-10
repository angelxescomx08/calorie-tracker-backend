#pragma once
#include <memory>
#include <functional>
#include "domain/repositories/IUserGoalRepository.hpp"

namespace application {

class GetActiveGoalUseCase : public std::enable_shared_from_this<GetActiveGoalUseCase> {
    std::shared_ptr<domain::IUserGoalRepository> repo_;
public:
    explicit GetActiveGoalUseCase(std::shared_ptr<domain::IUserGoalRepository> repo)
        : repo_(std::move(repo)) {}

    void execute(
        int64_t userId,
        std::function<void(std::optional<domain::UserGoal>)> onSuccess,
        std::function<void(std::string)>                     onError)
    {
        repo_->findActiveByUserId(userId, std::move(onSuccess), std::move(onError));
    }
};

} // namespace application
