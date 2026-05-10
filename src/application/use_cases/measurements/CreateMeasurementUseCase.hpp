#pragma once
#include <memory>
#include <functional>
#include <cmath>
#include "domain/repositories/IBodyMeasurementRepository.hpp"
#include "domain/repositories/IUserProfileRepository.hpp"

namespace application {

// Computes body fat % using US Navy formula then stores the measurement.
class CreateMeasurementUseCase : public std::enable_shared_from_this<CreateMeasurementUseCase> {
    std::shared_ptr<domain::IBodyMeasurementRepository> measureRepo_;
    std::shared_ptr<domain::IUserProfileRepository>     profileRepo_;
public:
    CreateMeasurementUseCase(
        std::shared_ptr<domain::IBodyMeasurementRepository> measureRepo,
        std::shared_ptr<domain::IUserProfileRepository>     profileRepo)
        : measureRepo_(std::move(measureRepo))
        , profileRepo_(std::move(profileRepo))
    {}

    void execute(
        domain::BodyMeasurement measurement,
        std::function<void(domain::BodyMeasurement)> onSuccess,
        std::function<void(std::string)>             onError)
    {
        auto self         = shared_from_this();
        auto onSuccessPtr = std::make_shared<decltype(onSuccess)>(std::move(onSuccess));
        auto onErrorPtr   = std::make_shared<decltype(onError)>(std::move(onError));

        // Fetch profile to get height (needed for US Navy formula).
        profileRepo_->findByUserId(measurement.user_id,
            [self, measurement, onSuccessPtr, onErrorPtr]
            (std::optional<domain::UserProfile> profileOpt) mutable {
                if (profileOpt) {
                    double height = profileOpt->height_cm;
                    bool isFemale = (profileOpt->gender == domain::Gender::Female);
                    measurement.body_fat_percentage = computeBodyFat(measurement, height, isFemale);
                }
                self->measureRepo_->create(std::move(measurement),
                    [onSuccessPtr](domain::BodyMeasurement m) { (*onSuccessPtr)(std::move(m)); },
                    [onErrorPtr](std::string err)              { (*onErrorPtr)(std::move(err)); });
            },
            [self, measurement, onSuccessPtr, onErrorPtr](std::string) mutable {
                // If profile not found, store without body fat %.
                self->measureRepo_->create(std::move(measurement),
                    [onSuccessPtr](domain::BodyMeasurement m) { (*onSuccessPtr)(std::move(m)); },
                    [onErrorPtr](std::string err)              { (*onErrorPtr)(std::move(err)); });
            });
    }

private:
    static std::optional<double> computeBodyFat(
        const domain::BodyMeasurement& m, double heightCm, bool isFemale)
    {
        if (heightCm <= 0) return std::nullopt;
        if (isFemale && m.hip_cm.has_value()) {
            double val = 163.205 * std::log10(m.waist_cm + *m.hip_cm - m.neck_cm)
                       - 97.684  * std::log10(heightCm)
                       - 78.387;
            return val > 0 ? std::optional<double>(val) : std::nullopt;
        } else if (!isFemale) {
            double val = 86.010  * std::log10(m.waist_cm - m.neck_cm)
                       - 70.041  * std::log10(heightCm)
                       + 36.76;
            return val > 0 ? std::optional<double>(val) : std::nullopt;
        }
        return std::nullopt;
    }
};

} // namespace application
