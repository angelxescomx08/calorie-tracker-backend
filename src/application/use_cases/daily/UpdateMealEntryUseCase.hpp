#pragma once
#include <memory>
#include <functional>
#include "domain/repositories/IMealEntryRepository.hpp"
#include "domain/repositories/IFoodRepository.hpp"

namespace application {

class UpdateMealEntryUseCase : public std::enable_shared_from_this<UpdateMealEntryUseCase> {
    std::shared_ptr<domain::IMealEntryRepository> entryRepo_;
    std::shared_ptr<domain::IFoodRepository>      foodRepo_;
public:
    UpdateMealEntryUseCase(
        std::shared_ptr<domain::IMealEntryRepository> entryRepo,
        std::shared_ptr<domain::IFoodRepository>      foodRepo)
        : entryRepo_(std::move(entryRepo))
        , foodRepo_(std::move(foodRepo))
    {}

    void execute(
        domain::MealEntry entry,
        std::function<void(domain::MealEntry)>   onSuccess,
        std::function<void(std::string)>         onError)
    {
        auto self         = shared_from_this();
        auto onSuccessPtr = std::make_shared<decltype(onSuccess)>(std::move(onSuccess));
        auto onErrorPtr   = std::make_shared<decltype(onError)>(std::move(onError));

        // Recompute snapshot macros from current food data.
        foodRepo_->findById(entry.food_id,
            [self, entry, onSuccessPtr, onErrorPtr](std::optional<domain::Food> foodOpt) mutable {
                if (foodOpt) {
                    double ratio   = entry.quantity_g / foodOpt->serving_size_g;
                    entry.calories = foodOpt->calories  * ratio;
                    entry.protein_g= foodOpt->protein_g * ratio;
                    entry.carbs_g  = foodOpt->carbs_g   * ratio;
                    entry.fat_g    = foodOpt->fat_g     * ratio;
                }
                self->entryRepo_->update(std::move(entry),
                    [onSuccessPtr](domain::MealEntry e) { (*onSuccessPtr)(std::move(e)); },
                    [onErrorPtr](std::string err)        { (*onErrorPtr)(std::move(err)); });
            },
            [onErrorPtr](std::string err) { (*onErrorPtr)(std::move(err)); });
    }
};

} // namespace application
