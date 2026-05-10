#include "application/use_cases/SeedExercisesUseCase.hpp"

namespace application {

SeedExercisesUseCase::SeedExercisesUseCase(
    std::shared_ptr<IExerciseLoader>             loader,
    std::shared_ptr<domain::IExerciseRepository> repository
)
    : loader_(std::move(loader))
    , repository_(std::move(repository))
{}

void SeedExercisesUseCase::execute(
    std::function<void(int)>         onSuccess,
    std::function<void(std::string)> onError
)
{
    std::vector<domain::Exercise> exercises;
    std::string                   error;

    if (!loader_->load(exercises, error)) {
        onError(error);
        return;
    }

    if (exercises.empty()) {
        onError("Loader returned zero exercises");
        return;
    }

    repository_->insertBatch(
        std::move(exercises),
        std::move(onSuccess),
        std::move(onError)
    );
}

} // namespace application
