#pragma once
#include <memory>
#include <functional>
#include <string>
#include "domain/repositories/IBodyMeasurementRepository.hpp"

namespace application {

class DeleteMeasurementUseCase : public std::enable_shared_from_this<DeleteMeasurementUseCase> {
    std::shared_ptr<domain::IBodyMeasurementRepository> repo_;
public:
    explicit DeleteMeasurementUseCase(std::shared_ptr<domain::IBodyMeasurementRepository> repo)
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
