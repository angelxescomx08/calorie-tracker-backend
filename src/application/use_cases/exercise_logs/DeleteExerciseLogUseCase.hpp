#pragma once
#include <memory>
#include <functional>
#include <string>
#include "domain/repositories/IExerciseLogRepository.hpp"

namespace application {

class DeleteExerciseLogUseCase : public std::enable_shared_from_this<DeleteExerciseLogUseCase> {
    std::shared_ptr<domain::IExerciseLogRepository> repo_;
public:
    explicit DeleteExerciseLogUseCase(std::shared_ptr<domain::IExerciseLogRepository> repo)
        : repo_(std::move(repo)) {}

    void execute(
        int64_t id, int64_t userId,
        std::function<void()>            onSuccess,
        std::function<void(std::string)> onError)
    {
        repo_->remove(id, userId, std::move(onSuccess), std::move(onError));
    }
};

} // namespace application
