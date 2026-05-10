#pragma once
#include <memory>
#include <functional>
#include "domain/repositories/IUserGoalRepository.hpp"

namespace application {

class CreateGoalUseCase : public std::enable_shared_from_this<CreateGoalUseCase> {
    std::shared_ptr<domain::IUserGoalRepository> repo_;
public:
    explicit CreateGoalUseCase(std::shared_ptr<domain::IUserGoalRepository> repo)
        : repo_(std::move(repo)) {}

    void execute(
        domain::UserGoal goal,
        std::function<void(domain::UserGoal)>    onSuccess,
        std::function<void(std::string)>         onError)
    {
        repo_->create(std::move(goal), std::move(onSuccess), std::move(onError));
    }
};

} // namespace application
