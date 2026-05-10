#pragma once
#include <string>
#include <cstdint>
#include <optional>

namespace domain {

struct BodyMeasurement {
    int64_t                  id;
    int64_t                  user_id;
    std::string              measured_date;      // YYYY-MM-DD
    double                   neck_cm;
    double                   waist_cm;
    std::optional<double>    hip_cm;             // women only
    std::optional<double>    body_fat_percentage; // calculated on insert
    std::string              notes;
    std::string              created_at;
};

} // namespace domain
