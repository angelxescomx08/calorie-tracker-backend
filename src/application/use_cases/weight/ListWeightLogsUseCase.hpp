#pragma once
#include <memory>
#include <functional>
#include <string>
#include "domain/repositories/IWeightLogRepository.hpp"

namespace application {

class ListWeightLogsUseCase : public std::enable_shared_from_this<ListWeightLogsUseCase> {
    std::shared_ptr<domain::IWeightLogRepository> repo_;
public:
    explicit ListWeightLogsUseCase(std::shared_ptr<domain::IWeightLogRepository> repo)
        : repo_(std::move(repo)) {}

    void execute(
        int64_t userId, const std::string& startDate, const std::string& endDate, int limit,
        std::function<void(std::vector<domain::WeightLog>)> onSuccess,
        std::function<void(std::string)>                    onError)
    {
        repo_->listByUserId(userId, startDate, endDate, limit,
            std::move(onSuccess), std::move(onError));
    }
};

} // namespace application
