#pragma once
#include <memory>
#include <functional>
#include "domain/repositories/IExerciseRepository.hpp"

namespace application {

class GetExerciseUseCase : public std::enable_shared_from_this<GetExerciseUseCase> {
    std::shared_ptr<domain::IExerciseRepository> repo_;
public:
    explicit GetExerciseUseCase(std::shared_ptr<domain::IExerciseRepository> repo)
        : repo_(std::move(repo)) {}

    void execute(
        int64_t id,
        std::function<void(std::optional<domain::Exercise>)> onSuccess,
        std::function<void(std::string)>                     onError)
    {
        repo_->findById(id, std::move(onSuccess), std::move(onError));
    }
};

} // namespace application
