#pragma once
#include <vector>
#include <functional>
#include <optional>
#include <string>
#include "domain/entities/Exercise.hpp"

namespace domain {

class IExerciseRepository {
public:
    virtual ~IExerciseRepository() = default;

    // Bulk insert used by the seed endpoint.
    virtual void insertBatch(
        std::vector<Exercise>            exercises,
        std::function<void(int)>         onSuccess,
        std::function<void(std::string)> onError) = 0;

    virtual void search(
        std::string query,
        std::string category, // empty = all categories
        int         limit,
        int         offset,
        std::function<void(std::vector<Exercise>, int total)> onSuccess,
        std::function<void(std::string)>                      onError) = 0;

    virtual void findById(
        int64_t id,
        std::function<void(std::optional<Exercise>)> onSuccess,
        std::function<void(std::string)>             onError) = 0;

    virtual void create(
        Exercise exercise,
        std::function<void(Exercise)>    onSuccess,
        std::function<void(std::string)> onError) = 0;
};

} // namespace domain
