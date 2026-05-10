#pragma once
#include <memory>
#include <functional>
#include <string>
#include "domain/repositories/IExerciseRepository.hpp"

namespace application {

class SearchExercisesUseCase : public std::enable_shared_from_this<SearchExercisesUseCase> {
    std::shared_ptr<domain::IExerciseRepository> repo_;
public:
    explicit SearchExercisesUseCase(std::shared_ptr<domain::IExerciseRepository> repo)
        : repo_(std::move(repo)) {}

    void execute(
        const std::string& query, const std::string& category, int limit, int offset,
        std::function<void(std::vector<domain::Exercise>, int total)> onSuccess,
        std::function<void(std::string)>                              onError)
    {
        repo_->search(query, category, limit, offset, std::move(onSuccess), std::move(onError));
    }
};

} // namespace application
