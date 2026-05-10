#pragma once
#include <string>
#include <cstdint>
#include "domain/enums/GoalType.hpp"

namespace domain {

struct UserGoal {
    int64_t     id;
    int64_t     user_id;
    GoalType    goal_type;
    double      target_weight_kg;
    double      weekly_rate_kg;
    int         daily_calorie_target;
    std::string start_date;        // YYYY-MM-DD
    std::string end_date;          // YYYY-MM-DD, empty = no limit
    bool        is_active;
    std::string created_at;
    std::string updated_at;
};

} // namespace domain
