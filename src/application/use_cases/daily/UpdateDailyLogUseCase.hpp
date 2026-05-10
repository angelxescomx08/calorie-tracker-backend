#pragma once
#include <memory>
#include <functional>
#include "domain/repositories/IDailyLogRepository.hpp"

namespace application {

class UpdateDailyLogUseCase : public std::enable_shared_from_this<UpdateDailyLogUseCase> {
    std::shared_ptr<domain::IDailyLogRepository> repo_;
public:
    explicit UpdateDailyLogUseCase(std::shared_ptr<domain::IDailyLogRepository> repo)
        : repo_(std::move(repo)) {}

    void execute(
        domain::DailyLog log,
        std::function<void(domain::DailyLog)>    onSuccess,
        std::function<void(std::string)>         onError)
    {
        repo_->update(std::move(log), std::move(onSuccess), std::move(onError));
    }
};

} // namespace application
