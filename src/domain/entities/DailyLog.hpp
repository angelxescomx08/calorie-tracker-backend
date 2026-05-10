#pragma once
#include <string>
#include <cstdint>

namespace domain {

struct DailyLog {
    int64_t     id;
    int64_t     user_id;
    std::string log_date;   // YYYY-MM-DD
    int         water_ml;
    std::string notes;
    std::string created_at;
    std::string updated_at;
};

} // namespace domain
