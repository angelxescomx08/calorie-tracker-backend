#pragma once
#include <memory>
#include <functional>
#include <string>
#include "domain/repositories/IUserGoalRepository.hpp"

namespace application {

class DeactivateGoalUseCase : public std::enable_shared_from_this<DeactivateGoalUseCase> {
    std::shared_ptr<domain::IUserGoalRepository> repo_;
public:
    explicit DeactivateGoalUseCase(std::shared_ptr<domain::IUserGoalRepository> repo)
        : repo_(std::move(repo)) {}

    void execute(
        int64_t goalId, int64_t userId,
        std::function<void()>            onSuccess,
        std::function<void(std::string)> onError)
    {
        repo_->deactivate(goalId, userId, std::move(onSuccess), std::move(onError));
    }
};

} // namespace application
