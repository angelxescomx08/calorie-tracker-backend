#pragma once
#include <memory>
#include <functional>
#include <string>
#include "domain/repositories/IBodyMeasurementRepository.hpp"

namespace application {

class ListMeasurementsUseCase : public std::enable_shared_from_this<ListMeasurementsUseCase> {
    std::shared_ptr<domain::IBodyMeasurementRepository> repo_;
public:
    explicit ListMeasurementsUseCase(std::shared_ptr<domain::IBodyMeasurementRepository> repo)
        : repo_(std::move(repo)) {}

    void execute(
        int64_t userId, const std::string& startDate, const std::string& endDate,
        std::function<void(std::vector<domain::BodyMeasurement>)> onSuccess,
        std::function<void(std::string)>                          onError)
    {
        repo_->listByUserId(userId, startDate, endDate, std::move(onSuccess), std::move(onError));
    }
};

} // namespace application
