#pragma once
#include <string>
#include <cstdint>
#include <optional>

namespace domain {

struct ExerciseLog {
    int64_t                id;
    int64_t                user_id;
    int64_t                exercise_id;
    std::string            log_date;        // YYYY-MM-DD
    int                    duration_minutes;
    double                 calories_burned; // snapshot: MET × weight_kg × hours
    std::optional<int>     sets;
    std::optional<int>     reps;
    std::optional<double>  distance_km;
    std::optional<int>     avg_heart_rate;
    std::string            notes;
    std::string            created_at;
};

} // namespace domain
