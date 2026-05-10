#pragma once
#include <memory>
#include <functional>
#include <string>
#include "application/ports/IExerciseLoader.hpp"
#include "domain/repositories/IExerciseRepository.hpp"

namespace application {

class SeedExercisesUseCase {
public:
    SeedExercisesUseCase(
        std::shared_ptr<IExerciseLoader>             loader,
        std::shared_ptr<domain::IExerciseRepository> repository
    );

    void execute(
        std::function<void(int)>         onSuccess,
        std::function<void(std::string)> onError
    );

private:
    std::shared_ptr<IExerciseLoader>             loader_;
    std::shared_ptr<domain::IExerciseRepository> repository_;
};

} // namespace application
