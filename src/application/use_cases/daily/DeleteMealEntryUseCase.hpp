#pragma once
#include <memory>
#include <functional>
#include <string>
#include "domain/repositories/IMealEntryRepository.hpp"

namespace application {

class DeleteMealEntryUseCase : public std::enable_shared_from_this<DeleteMealEntryUseCase> {
    std::shared_ptr<domain::IMealEntryRepository> repo_;
public:
    explicit DeleteMealEntryUseCase(std::shared_ptr<domain::IMealEntryRepository> repo)
        : repo_(std::move(repo)) {}

    void execute(
        int64_t id, int64_t dailyLogId,
        std::function<void()>            onSuccess,
        std::function<void(std::string)> onError)
    {
        repo_->remove(id, dailyLogId, std::move(onSuccess), std::move(onError));
    }
};

} // namespace application
