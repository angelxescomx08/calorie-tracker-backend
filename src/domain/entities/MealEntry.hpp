#pragma once
#include <string>
#include <cstdint>
#include "domain/enums/MealType.hpp"

namespace domain {

struct MealEntry {
    int64_t     id;
    int64_t     daily_log_id;
    MealType    meal_type;
    int64_t     food_id;
    double      quantity_g;
    double      calories;   // snapshot
    double      protein_g;  // snapshot
    double      carbs_g;    // snapshot
    double      fat_g;      // snapshot
    std::string created_at;
};

} // namespace domain
