#pragma once
#include <memory>
#include <functional>
#include "domain/repositories/IFoodRepository.hpp"

namespace application {

class CreateFoodUseCase : public std::enable_shared_from_this<CreateFoodUseCase> {
    std::shared_ptr<domain::IFoodRepository> repo_;
public:
    explicit CreateFoodUseCase(std::shared_ptr<domain::IFoodRepository> repo)
        : repo_(std::move(repo)) {}

    void execute(
        domain::Food food,
        std::function<void(domain::Food)>    onSuccess,
        std::function<void(std::string)>     onError)
    {
        repo_->create(std::move(food), std::move(onSuccess), std::move(onError));
    }
};

} // namespace application
