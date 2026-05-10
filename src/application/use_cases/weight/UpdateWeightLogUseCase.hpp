#pragma once
#include <memory>
#include <functional>
#include "domain/repositories/IWeightLogRepository.hpp"

namespace application {

class UpdateWeightLogUseCase : public std::enable_shared_from_this<UpdateWeightLogUseCase> {
    std::shared_ptr<domain::IWeightLogRepository> repo_;
public:
    explicit UpdateWeightLogUseCase(std::shared_ptr<domain::IWeightLogRepository> repo)
        : repo_(std::move(repo)) {}

    void execute(
        domain::WeightLog log,
        std::function<void(domain::WeightLog)>   onSuccess,
        std::function<void(std::string)>         onError)
    {
        repo_->update(std::move(log), std::move(onSuccess), std::move(onError));
    }
};

} // namespace application
