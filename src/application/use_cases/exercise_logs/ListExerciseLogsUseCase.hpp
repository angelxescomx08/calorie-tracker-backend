#pragma once
#include <memory>
#include <functional>
#include <string>
#include "domain/repositories/IExerciseLogRepository.hpp"

namespace application {

class ListExerciseLogsUseCase : public std::enable_shared_from_this<ListExerciseLogsUseCase> {
    std::shared_ptr<domain::IExerciseLogRepository> repo_;
public:
    explicit ListExerciseLogsUseCase(std::shared_ptr<domain::IExerciseLogRepository> repo)
        : repo_(std::move(repo)) {}

    void execute(
        int64_t userId, const std::string& date,
        std::function<void(std::vector<domain::ExerciseLog>)> onSuccess,
        std::function<void(std::string)>                      onError)
    {
        repo_->listByUserAndDate(userId, date, std::move(onSuccess), std::move(onError));
    }
};

} // namespace application
