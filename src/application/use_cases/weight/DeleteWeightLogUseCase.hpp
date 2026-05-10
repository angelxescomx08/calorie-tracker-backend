#pragma once
#include <memory>
#include <functional>
#include <string>
#include "domain/repositories/IWeightLogRepository.hpp"

namespace application {

class DeleteWeightLogUseCase : public std::enable_shared_from_this<DeleteWeightLogUseCase> {
    std::shared_ptr<domain::IWeightLogRepository> repo_;
public:
    explicit DeleteWeightLogUseCase(std::shared_ptr<domain::IWeightLogRepository> repo)
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
