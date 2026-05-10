#pragma once
#include <memory>
#include <functional>
#include "domain/repositories/IExerciseLogRepository.hpp"

namespace application {

class UpdateExerciseLogUseCase : public std::enable_shared_from_this<UpdateExerciseLogUseCase> {
    std::shared_ptr<domain::IExerciseLogRepository> repo_;
public:
    explicit UpdateExerciseLogUseCase(std::shared_ptr<domain::IExerciseLogRepository> repo)
        : repo_(std::move(repo)) {}

    void execute(
        domain::ExerciseLog log,
        std::function<void(domain::ExerciseLog)> onSuccess,
        std::function<void(std::string)>         onError)
    {
        repo_->update(std::move(log), std::move(onSuccess), std::move(onError));
    }
};

} // namespace application
