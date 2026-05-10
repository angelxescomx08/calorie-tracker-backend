#pragma once
#include <memory>
#include <drogon/orm/DbClient.h>
#include "domain/repositories/IBodyMeasurementRepository.hpp"

namespace infrastructure {

class PostgresBodyMeasurementRepository
    : public domain::IBodyMeasurementRepository
    , public std::enable_shared_from_this<PostgresBodyMeasurementRepository>
{
public:
    explicit PostgresBodyMeasurementRepository(drogon::orm::DbClientPtr db);

    void listByUserId(int64_t userId,
        std::string startDate, std::string endDate,
        std::function<void(std::vector<domain::BodyMeasurement>)> onSuccess,
        std::function<void(std::string)>                          onError) override;

    void create(domain::BodyMeasurement measurement,
        std::function<void(domain::BodyMeasurement)> onSuccess,
        std::function<void(std::string)>             onError) override;

    void remove(int64_t id, int64_t userId,
        std::function<void()>            onSuccess,
        std::function<void(std::string)> onError) override;

private:
    drogon::orm::DbClientPtr db_;
    static domain::BodyMeasurement rowToMeasurement(const drogon::orm::Row& row);
};

} // namespace infrastructure
