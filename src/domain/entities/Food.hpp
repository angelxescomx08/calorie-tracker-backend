#pragma once
#include <string>
#include <cstdint>

namespace domain {

struct Food {
    int64_t     id;
    std::string name;
    std::string brand;      // empty = not set
    std::string barcode;    // empty = not set
    double      serving_size_g;
    double      calories;
    double      protein_g;
    double      carbs_g;
    double      fat_g;
    double      fiber_g;
    double      sugar_g;
    double      sodium_mg;
    int64_t     created_by_user_id; // 0 = system/global
    bool        is_verified;
    std::string created_at;
    std::string updated_at;
};

} // namespace domain
