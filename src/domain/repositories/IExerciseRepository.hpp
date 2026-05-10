#pragma once
#include <vector>
#include <functional>
#include <string>
#include "domain/entities/Exercise.hpp"

namespace domain {

class IExerciseRepository {
public:
    virtual ~IExerciseRepository() = default;

    // Async. Exactly one callback fires after the operation completes.
    virtual void insertBatch(
        std::vector<Exercise>            exercises,
        std::function<void(int)>         onSuccess,
        std::function<void(std::string)> onError
    ) = 0;
};

} // namespace domain
