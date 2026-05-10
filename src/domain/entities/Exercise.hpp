#pragma once
#include <string>
#include <cstdint>
#include "domain/enums/ExerciseCategory.hpp"

namespace domain {

struct Exercise {
    int64_t          id;           // 0 = not yet persisted
    std::string      name;
    ExerciseCategory category;
    double           met_value;
    std::string      description;
    bool             is_verified;
    int64_t          created_by_user_id; // 0 = system
    std::string      created_at;
};

} // namespace domain
