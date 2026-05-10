#pragma once
#include <memory>
#include <functional>
#include <string>
#include "domain/repositories/IFoodRepository.hpp"

namespace application {

class DeleteFoodUseCase : public std::enable_shared_from_this<DeleteFoodUseCase> {
    std::shared_ptr<domain::IFoodRepository> repo_;
public:
    explicit DeleteFoodUseCase(std::shared_ptr<domain::IFoodRepository> repo)
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
