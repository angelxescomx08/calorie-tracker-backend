#pragma once
#include <memory>
#include <functional>
#include "domain/repositories/IExerciseRepository.hpp"

namespace application {

class CreateExerciseUseCase : public std::enable_shared_from_this<CreateExerciseUseCase> {
    std::shared_ptr<domain::IExerciseRepository> repo_;
public:
    explicit CreateExerciseUseCase(std::shared_ptr<domain::IExerciseRepository> repo)
        : repo_(std::move(repo)) {}

    void execute(
        domain::Exercise exercise,
        std::function<void(domain::Exercise)>    onSuccess,
        std::function<void(std::string)>         onError)
    {
        repo_->create(std::move(exercise), std::move(onSuccess), std::move(onError));
    }
};

} // namespace application
