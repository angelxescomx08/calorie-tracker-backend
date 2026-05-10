#include "infrastructure/persistence/PostgresExerciseRepository.hpp"
#include <sstream>

namespace infrastructure {

PostgresExerciseRepository::PostgresExerciseRepository(
    drogon::orm::DbClientPtr dbClient)
    : dbClient_(std::move(dbClient))
{}

domain::Exercise PostgresExerciseRepository::rowToExercise(const drogon::orm::Row& row)
{
    domain::Exercise e;
    e.id                   = row["id"].as<int64_t>();
    e.name                 = row["name"].as<std::string>();
    e.met_value            = row["met_value"].as<double>();
    e.description          = row["description"].isNull() ? "" : row["description"].as<std::string>();
    e.is_verified          = row["is_verified"].as<bool>();
    e.created_by_user_id   = row["created_by_user_id"].isNull() ? 0 : row["created_by_user_id"].as<int64_t>();
    e.created_at           = row["created_at"].as<std::string>();

    auto cat = row["category"].as<std::string>();
    if      (cat == "cardio")      e.category = domain::ExerciseCategory::Cardio;
    else if (cat == "strength")    e.category = domain::ExerciseCategory::Strength;
    else if (cat == "flexibility") e.category = domain::ExerciseCategory::Flexibility;
    else if (cat == "sports")      e.category = domain::ExerciseCategory::Sports;
    else                           e.category = domain::ExerciseCategory::Other;

    return e;
}

void PostgresExerciseRepository::insertBatch(
    std::vector<domain::Exercise>    exercises,
    std::function<void(int)>         onSuccess,
    std::function<void(std::string)> onError)
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
        });
}

void PostgresExerciseRepository::search(
    std::string query, std::string category, int limit, int offset,
    std::function<void(std::vector<domain::Exercise>, int total)> onSuccess,
    std::function<void(std::string)>                              onError)
{
    // Escape query.
    std::string q;
    for (char c : query) { if (c == '\'') q += "''"; else q += c; }

    std::string where = "TRUE";
    if (!q.empty())
        where += " AND (name ILIKE '%" + q + "%' OR description ILIKE '%" + q + "%')";
    if (!category.empty())
        where += " AND category = '" + category + "'::exercise_category";

    std::string countSql = "SELECT COUNT(*) FROM exercises WHERE " + where;
    std::string dataSql =
        "SELECT id, name, category, met_value, description, created_by_user_id,"
        " is_verified, created_at::text FROM exercises WHERE " + where +
        " ORDER BY name LIMIT " + std::to_string(limit) +
        " OFFSET " + std::to_string(offset);

    auto self        = shared_from_this();
    auto onSuccessPtr = std::make_shared<decltype(onSuccess)>(std::move(onSuccess));
    auto onErrorPtr   = std::make_shared<decltype(onError)>(std::move(onError));

    dbClient_->execSqlAsync(
        countSql,
        [self, dataSql, onSuccessPtr, onErrorPtr](const drogon::orm::Result& r) {
            int total = r[0][0].as<int>();
            self->dbClient_->execSqlAsync(
                dataSql,
                [onSuccessPtr, total](const drogon::orm::Result& r2) {
                    std::vector<domain::Exercise> exs;
                    exs.reserve(r2.size());
                    for (const auto& row : r2) exs.push_back(rowToExercise(row));
                    (*onSuccessPtr)(std::move(exs), total);
                },
                [onErrorPtr](const drogon::orm::DrogonDbException& e) {
                    (*onErrorPtr)(e.base().what());
                });
        },
        [onErrorPtr](const drogon::orm::DrogonDbException& e) {
            (*onErrorPtr)(e.base().what());
        });
}

void PostgresExerciseRepository::findById(
    int64_t id,
    std::function<void(std::optional<domain::Exercise>)> onSuccess,
    std::function<void(std::string)>                     onError)
{
    auto self = shared_from_this();
    dbClient_->execSqlAsync(
        "SELECT id, name, category, met_value, description, created_by_user_id,"
        " is_verified, created_at::text FROM exercises WHERE id = $1",
        [self, onSuccess = std::move(onSuccess)](const drogon::orm::Result& r) {
            if (r.empty()) { onSuccess(std::nullopt); return; }
            onSuccess(rowToExercise(r[0]));
        },
        [onError = std::move(onError)](const drogon::orm::DrogonDbException& e) {
            onError(e.base().what());
        },
        id);
}

void PostgresExerciseRepository::create(
    domain::Exercise exercise,
    std::function<void(domain::Exercise)>    onSuccess,
    std::function<void(std::string)>         onError)
{
    std::string catStr  = categoryToString(exercise.category);
    std::string descSql = exercise.description.empty()
        ? "NULL" : ("'" + sqlEscape(exercise.description) + "'");
    std::string ownerSql = exercise.created_by_user_id == 0
        ? "NULL" : std::to_string(exercise.created_by_user_id);

    std::string sql =
        "INSERT INTO exercises (name, category, met_value, description, created_by_user_id, is_verified)"
        " VALUES ($1, '" + catStr + "'::exercise_category, $2, " + descSql + ", " + ownerSql + ","
        " " + (exercise.is_verified ? "TRUE" : "FALSE") + ")"
        " RETURNING id, name, category, met_value, description, created_by_user_id,"
        "  is_verified, created_at::text";

    auto self = shared_from_this();
    dbClient_->execSqlAsync(
        sql,
        [self, onSuccess = std::move(onSuccess)](const drogon::orm::Result& r) {
            onSuccess(rowToExercise(r[0]));
        },
        [onError = std::move(onError)](const drogon::orm::DrogonDbException& e) {
            onError(e.base().what());
        },
        exercise.name, exercise.met_value);
}

std::string PostgresExerciseRepository::buildInsertSql(
    const std::vector<domain::Exercise>& exercises)
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
    sql << " ON CONFLICT DO NOTHING";
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
