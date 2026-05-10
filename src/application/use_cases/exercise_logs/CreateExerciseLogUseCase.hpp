#pragma once
#include <memory>
#include <functional>
#include "domain/repositories/IExerciseLogRepository.hpp"
#include "domain/repositories/IExerciseRepository.hpp"
#include "domain/repositories/IWeightLogRepository.hpp"

namespace application {

class CreateExerciseLogUseCase : public std::enable_shared_from_this<CreateExerciseLogUseCase> {
    std::shared_ptr<domain::IExerciseLogRepository> logRepo_;
    std::shared_ptr<domain::IExerciseRepository>    exerciseRepo_;
    std::shared_ptr<domain::IWeightLogRepository>   weightRepo_;
public:
    CreateExerciseLogUseCase(
        std::shared_ptr<domain::IExerciseLogRepository> logRepo,
        std::shared_ptr<domain::IExerciseRepository>    exerciseRepo,
        std::shared_ptr<domain::IWeightLogRepository>   weightRepo)
        : logRepo_(std::move(logRepo))
        , exerciseRepo_(std::move(exerciseRepo))
        , weightRepo_(std::move(weightRepo))
    {}

    void execute(
        domain::ExerciseLog log,
        std::function<void(domain::ExerciseLog)> onSuccess,
        std::function<void(std::string)>         onError)
    {
        auto self         = shared_from_this();
        auto onSuccessPtr = std::make_shared<decltype(onSuccess)>(std::move(onSuccess));
        auto onErrorPtr   = std::make_shared<decltype(onError)>(std::move(onError));

        // Fetch exercise to get MET, then fetch latest weight, then compute calories.
        exerciseRepo_->findById(log.exercise_id,
            [self, log, onSuccessPtr, onErrorPtr]
            (std::optional<domain::Exercise> exOpt) mutable {
                if (!exOpt) { (*onErrorPtr)("Exercise not found"); return; }
                double met = exOpt->met_value;

                self->weightRepo_->findLatest(log.user_id, log.log_date,
                    [self, log, met, onSuccessPtr, onErrorPtr]
                    (std::optional<domain::WeightLog> wOpt) mutable {
                        double weightKg = wOpt.has_value() ? wOpt->weight_kg : 70.0; // fallback
                        log.calories_burned = met * weightKg * (log.duration_minutes / 60.0);
                        self->logRepo_->create(std::move(log),
                            [onSuccessPtr](domain::ExerciseLog l) { (*onSuccessPtr)(std::move(l)); },
                            [onErrorPtr](std::string err)          { (*onErrorPtr)(std::move(err)); });
                    },
                    [self, log, met, onSuccessPtr, onErrorPtr](std::string) mutable {
                        // If weight lookup fails, use fallback.
                        log.calories_burned = met * 70.0 * (log.duration_minutes / 60.0);
                        self->logRepo_->create(std::move(log),
                            [onSuccessPtr](domain::ExerciseLog l) { (*onSuccessPtr)(std::move(l)); },
                            [onErrorPtr](std::string err)          { (*onErrorPtr)(std::move(err)); });
                    });
            },
            [onErrorPtr](std::string err) { (*onErrorPtr)(std::move(err)); });
    }
};

} // namespace application
