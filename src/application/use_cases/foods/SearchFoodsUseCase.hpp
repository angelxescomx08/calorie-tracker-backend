#pragma once
#include <memory>
#include <functional>
#include <string>
#include "domain/repositories/IFoodRepository.hpp"

namespace application {

class SearchFoodsUseCase : public std::enable_shared_from_this<SearchFoodsUseCase> {
    std::shared_ptr<domain::IFoodRepository> repo_;
public:
    explicit SearchFoodsUseCase(std::shared_ptr<domain::IFoodRepository> repo)
        : repo_(std::move(repo)) {}

    void execute(
        const std::string& query, int limit, int offset,
        std::function<void(std::vector<domain::Food>, int total)> onSuccess,
        std::function<void(std::string)>                          onError)
    {
        repo_->search(query, limit, offset, std::move(onSuccess), std::move(onError));
    }
};

} // namespace application
