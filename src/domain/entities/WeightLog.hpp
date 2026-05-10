#pragma once
#include <string>
#include <cstdint>

namespace domain {

struct WeightLog {
    int64_t     id;
    int64_t     user_id;
    double      weight_kg;
    std::string logged_date; // YYYY-MM-DD
    std::string notes;       // empty = not set
    std::string created_at;
};

} // namespace domain
