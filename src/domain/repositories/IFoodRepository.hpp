#pragma once
#include <functional>
#include <optional>
#include <vector>
#include <string>
#include "domain/entities/Food.hpp"

namespace domain {

class IFoodRepository {
public:
    virtual ~IFoodRepository() = default;

    virtual void search(
        std::string query,
        int         limit,
        int         offset,
        std::function<void(std::vector<Food>, int total)> onSuccess,
        std::function<void(std::string)>                  onError) = 0;

    virtual void findById(
        int64_t id,
        std::function<void(std::optional<Food>)> onSuccess,
        std::function<void(std::string)>         onError) = 0;

    virtual void create(
        Food food,
        std::function<void(Food)>        onSuccess,
        std::function<void(std::string)> onError) = 0;

    virtual void update(
        Food food,
        std::function<void(Food)>        onSuccess,
        std::function<void(std::string)> onError) = 0;

    // Only allows deletion of foods owned by userId.
    virtual void remove(
        int64_t id,
        int64_t userId,
        std::function<void()>            onSuccess,
        std::function<void(std::string)> onError) = 0;
};

} // namespace domain
