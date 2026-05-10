#pragma once
#include <memory>
#include <functional>
#include "domain/repositories/IDailyLogRepository.hpp"
#include "domain/repositories/IMealEntryRepository.hpp"
#include "domain/repositories/IFoodRepository.hpp"

namespace application {

class AddMealEntryUseCase : public std::enable_shared_from_this<AddMealEntryUseCase> {
    std::shared_ptr<domain::IDailyLogRepository>  logRepo_;
    std::shared_ptr<domain::IMealEntryRepository> entryRepo_;
    std::shared_ptr<domain::IFoodRepository>      foodRepo_;
public:
    AddMealEntryUseCase(
        std::shared_ptr<domain::IDailyLogRepository>  logRepo,
        std::shared_ptr<domain::IMealEntryRepository> entryRepo,
        std::shared_ptr<domain::IFoodRepository>      foodRepo)
        : logRepo_(std::move(logRepo))
        , entryRepo_(std::move(entryRepo))
        , foodRepo_(std::move(foodRepo))
    {}

    void execute(
        int64_t userId, const std::string& date,
        domain::MealType mealType, int64_t foodId, double quantityG,
        std::function<void(domain::MealEntry)>   onSuccess,
        std::function<void(std::string)>         onError)
    {
        auto self         = shared_from_this();
        auto onSuccessPtr = std::make_shared<decltype(onSuccess)>(std::move(onSuccess));
        auto onErrorPtr   = std::make_shared<decltype(onError)>(std::move(onError));

        // 1. Get or create daily log.
        logRepo_->getOrCreate(userId, date,
            [self, mealType, foodId, quantityG, onSuccessPtr, onErrorPtr](domain::DailyLog log) {
                // 2. Fetch food to compute snapshot macros.
                self->foodRepo_->findById(foodId,
                    [self, log, mealType, foodId, quantityG, onSuccessPtr, onErrorPtr]
                    (std::optional<domain::Food> foodOpt) {
                        if (!foodOpt) { (*onErrorPtr)("Food not found"); return; }
                        const domain::Food& food = *foodOpt;
                        double ratio = quantityG / food.serving_size_g;
                        domain::MealEntry entry{};
                        entry.daily_log_id = log.id;
                        entry.meal_type    = mealType;
                        entry.food_id      = foodId;
                        entry.quantity_g   = quantityG;
                        entry.calories     = food.calories  * ratio;
                        entry.protein_g    = food.protein_g * ratio;
                        entry.carbs_g      = food.carbs_g   * ratio;
                        entry.fat_g        = food.fat_g     * ratio;
                        self->entryRepo_->create(std::move(entry),
                            [onSuccessPtr](domain::MealEntry e) { (*onSuccessPtr)(std::move(e)); },
                            [onErrorPtr](std::string err)        { (*onErrorPtr)(std::move(err)); });
                    },
                    [onErrorPtr](std::string err) { (*onErrorPtr)(std::move(err)); });
            },
            [onErrorPtr](std::string err) { (*onErrorPtr)(std::move(err)); });
    }
};

} // namespace application
