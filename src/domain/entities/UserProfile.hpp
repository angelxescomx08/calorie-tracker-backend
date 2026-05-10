#pragma once
#include <string>
#include <cstdint>
#include "domain/enums/Gender.hpp"
#include "domain/enums/ActivityLevel.hpp"

namespace domain {

struct UserProfile {
    int64_t       id;
    int64_t       user_id;
    std::string   birth_date;    // YYYY-MM-DD
    Gender        gender;
    double        height_cm;
    ActivityLevel activity_level;
    std::string   created_at;
    std::string   updated_at;
};

} // namespace domain
