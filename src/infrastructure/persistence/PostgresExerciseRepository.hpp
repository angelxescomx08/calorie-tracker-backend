#pragma once
#include <memory>
#include <drogon/orm/DbClient.h>
#include "domain/repositories/IExerciseRepository.hpp"

namespace infrastructure {

class PostgresExerciseRepository
    : public domain::IExerciseRepository
    , public std::enable_shared_from_this<PostgresExerciseRepository>
{
public:
    explicit PostgresExerciseRepository(drogon::orm::DbClientPtr dbClient);

    void insertBatch(
        std::vector<domain::Exercise>    exercises,
        std::function<void(int)>         onSuccess,
        std::function<void(std::string)> onError) override;

    void search(
        std::string query, std::string category, int limit, int offset,
        std::function<void(std::vector<domain::Exercise>, int total)> onSuccess,
        std::function<void(std::string)>                              onError) override;

    void findById(
        int64_t id,
        std::function<void(std::optional<domain::Exercise>)> onSuccess,
        std::function<void(std::string)>                     onError) override;

    void create(
        domain::Exercise exercise,
        std::function<void(domain::Exercise)>    onSuccess,
        std::function<void(std::string)>         onError) override;

private:
    drogon::orm::DbClientPtr dbClient_;

    static domain::Exercise rowToExercise(const drogon::orm::Row& row);
    static std::string buildInsertSql(const std::vector<domain::Exercise>& exercises);
    static std::string categoryToString(domain::ExerciseCategory cat);
    static std::string sqlEscape(const std::string& s);
};

} // namespace infrastructure
