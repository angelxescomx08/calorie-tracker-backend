#pragma once
#include <memory>
#include <drogon/orm/DbClient.h>
#include "domain/repositories/IFoodRepository.hpp"

namespace infrastructure {

class PostgresFoodRepository
    : public domain::IFoodRepository
    , public std::enable_shared_from_this<PostgresFoodRepository>
{
public:
    explicit PostgresFoodRepository(drogon::orm::DbClientPtr db);

    void search(std::string query, int limit, int offset,
        std::function<void(std::vector<domain::Food>, int total)> onSuccess,
        std::function<void(std::string)>                          onError) override;

    void findById(int64_t id,
        std::function<void(std::optional<domain::Food>)> onSuccess,
        std::function<void(std::string)>                 onError) override;

    void create(domain::Food food,
        std::function<void(domain::Food)>    onSuccess,
        std::function<void(std::string)>     onError) override;

    void update(domain::Food food,
        std::function<void(domain::Food)>    onSuccess,
        std::function<void(std::string)>     onError) override;

    void remove(int64_t id, int64_t userId,
        std::function<void()>            onSuccess,
        std::function<void(std::string)> onError) override;

private:
    drogon::orm::DbClientPtr db_;
    static domain::Food rowToFood(const drogon::orm::Row& row);
};

} // namespace infrastructure
