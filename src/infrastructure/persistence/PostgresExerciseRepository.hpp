#pragma once
#include <memory>
#include <drogon/orm/DbClient.h>
#include "domain/repositories/IExerciseRepository.hpp"

namespace infrastructure {

// Adapter: maps Drogon's DbClientPtr to IExerciseRepository.
class PostgresExerciseRepository
    : public domain::IExerciseRepository
    , public std::enable_shared_from_this<PostgresExerciseRepository>
{
public:
    explicit PostgresExerciseRepository(drogon::orm::DbClientPtr dbClient);

    void insertBatch(
        std::vector<domain::Exercise>    exercises,
        std::function<void(int)>         onSuccess,
        std::function<void(std::string)> onError
    ) override;

private:
    drogon::orm::DbClientPtr dbClient_;

    static std::string buildInsertSql(const std::vector<domain::Exercise>& exercises);
    static std::string categoryToString(domain::ExerciseCategory cat);
    static std::string sqlEscape(const std::string& s);
};

} // namespace infrastructure
