#pragma once
#include <vector>
#include <string>
#include "domain/entities/Exercise.hpp"

namespace application {

class IExerciseLoader {
public:
    virtual ~IExerciseLoader() = default;

    // Synchronous. Returns false and sets errorOut on failure.
    virtual bool load(
        std::vector<domain::Exercise>& out,
        std::string&                   errorOut
    ) = 0;
};

} // namespace application
