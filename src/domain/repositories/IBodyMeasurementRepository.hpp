#pragma once
#include <functional>
#include <vector>
#include <string>
#include "domain/entities/BodyMeasurement.hpp"

namespace domain {

class IBodyMeasurementRepository {
public:
    virtual ~IBodyMeasurementRepository() = default;

    virtual void listByUserId(
        int64_t     userId,
        std::string startDate,
        std::string endDate,
        std::function<void(std::vector<BodyMeasurement>)> onSuccess,
        std::function<void(std::string)>                  onError) = 0;

    virtual void create(
        BodyMeasurement measurement,
        std::function<void(BodyMeasurement)> onSuccess,
        std::function<void(std::string)>     onError) = 0;

    virtual void remove(
        int64_t id,
        int64_t userId,
        std::function<void()>            onSuccess,
        std::function<void(std::string)> onError) = 0;
};

} // namespace domain
