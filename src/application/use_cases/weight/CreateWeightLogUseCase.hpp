#pragma once
#include <memory>
#include <functional>
#include "domain/repositories/IWeightLogRepository.hpp"

namespace application {

class CreateWeightLogUseCase : public std::enable_shared_from_this<CreateWeightLogUseCase> {
    std::shared_ptr<domain::IWeightLogRepository> repo_;
public:
    explicit CreateWeightLogUseCase(std::shared_ptr<domain::IWeightLogRepository> repo)
        : repo_(std::move(repo)) {}

    void execute(
        domain::WeightLog log,
        std::function<void(domain::WeightLog)>   onSuccess,
        std::function<void(std::string)>         onError)
    {
        repo_->create(std::move(log), std::move(onSuccess), std::move(onError));
    }
};

} // namespace application
