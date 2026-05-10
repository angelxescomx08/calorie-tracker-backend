#pragma once
#include <string>
#include "domain/enums/ExerciseCategory.hpp"

namespace domain {

struct Exercise {
    std::string      name;
    ExerciseCategory category;
    double           met_value;
    std::string      description;
    bool             is_verified;
};

} // namespace domain
