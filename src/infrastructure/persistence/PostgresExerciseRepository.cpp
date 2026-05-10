#include "infrastructure/persistence/PostgresExerciseRepository.hpp"
#include <sstream>

namespace infrastructure {

PostgresExerciseRepository::PostgresExerciseRepository(
    drogon::orm::DbClientPtr dbClient
)
    : dbClient_(std::move(dbClient))
{}

void PostgresExerciseRepository::insertBatch(
    std::vector<domain::Exercise>    exercises,
    std::function<void(int)>         onSuccess,
    std::function<void(std::string)> onError
)
{
    const int         count = static_cast<int>(exercises.size());
    const std::string sql   = buildInsertSql(exercises);
    auto              self  = shared_from_this();

    dbClient_->execSqlAsync(
        sql,
        [self, onSuccess = std::move(onSuccess), count]
        (const drogon::orm::Result&) {
            onSuccess(count);
        },
        [self, onError = std::move(onError)]
        (const drogon::orm::DrogonDbException& e) {
            onError(std::string(e.base().what()));
        }
    );
}

std::string PostgresExerciseRepository::buildInsertSql(
    const std::vector<domain::Exercise>& exercises
)
{
    std::ostringstream sql;
    sql << "INSERT INTO exercises "
           "(name, category, met_value, description, is_verified) VALUES ";

    bool first = true;
    for (const auto& ex : exercises) {
        if (!first) sql << ',';
        sql << "('"  << sqlEscape(ex.name)            << "',"
            << "'"   << categoryToString(ex.category) << "',"
            << ex.met_value                            << ","
            << "'"   << sqlEscape(ex.description)     << "',"
            << (ex.is_verified ? "TRUE" : "FALSE")    << ")";
        first = false;
    }
    return sql.str();
}

std::string PostgresExerciseRepository::categoryToString(domain::ExerciseCategory cat)
{
    switch (cat) {
        case domain::ExerciseCategory::Cardio:      return "cardio";
        case domain::ExerciseCategory::Strength:    return "strength";
        case domain::ExerciseCategory::Flexibility: return "flexibility";
        case domain::ExerciseCategory::Sports:      return "sports";
        default:                                    return "other";
    }
}

std::string PostgresExerciseRepository::sqlEscape(const std::string& s)
{
    std::string out;
    out.reserve(s.size() + 8);
    for (char c : s) {
        if (c == '\'') out += "''";
        else           out += c;
    }
    return out;
}

} // namespace infrastructure
