#pragma once
#include <memory>
#include <functional>
#include "domain/repositories/IFoodRepository.hpp"

namespace application {

class GetFoodUseCase : public std::enable_shared_from_this<GetFoodUseCase> {
    std::shared_ptr<domain::IFoodRepository> repo_;
public:
    explicit GetFoodUseCase(std::shared_ptr<domain::IFoodRepository> repo)
        : repo_(std::move(repo)) {}

    void execute(
        int64_t id,
        std::function<void(std::optional<domain::Food>)> onSuccess,
        std::function<void(std::string)>                 onError)
    {
        repo_->findById(id, std::move(onSuccess), std::move(onError));
    }
};

} // namespace application
