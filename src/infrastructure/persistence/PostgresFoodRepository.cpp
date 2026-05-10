#include "infrastructure/persistence/PostgresFoodRepository.hpp"
#include <sstream>

namespace infrastructure {

PostgresFoodRepository::PostgresFoodRepository(drogon::orm::DbClientPtr db)
    : db_(std::move(db)) {}

domain::Food PostgresFoodRepository::rowToFood(const drogon::orm::Row& row)
{
    domain::Food f;
    f.id                   = row["id"].as<int64_t>();
    f.name                 = row["name"].as<std::string>();
    f.brand                = row["brand"].isNull()   ? "" : row["brand"].as<std::string>();
    f.barcode              = row["barcode"].isNull() ? "" : row["barcode"].as<std::string>();
    f.serving_size_g       = row["serving_size_g"].as<double>();
    f.calories             = row["calories"].as<double>();
    f.protein_g            = row["protein_g"].as<double>();
    f.carbs_g              = row["carbs_g"].as<double>();
    f.fat_g                = row["fat_g"].as<double>();
    f.fiber_g              = row["fiber_g"].isNull() ? 0.0 : row["fiber_g"].as<double>();
    f.sugar_g              = row["sugar_g"].isNull() ? 0.0 : row["sugar_g"].as<double>();
    f.sodium_mg            = row["sodium_mg"].isNull() ? 0.0 : row["sodium_mg"].as<double>();
    f.created_by_user_id   = row["created_by_user_id"].isNull()
                             ? 0 : row["created_by_user_id"].as<int64_t>();
    f.is_verified          = row["is_verified"].as<bool>();
    f.created_at           = row["created_at"].as<std::string>();
    f.updated_at           = row["updated_at"].as<std::string>();
    return f;
}

void PostgresFoodRepository::search(
    std::string query, int limit, int offset,
    std::function<void(std::vector<domain::Food>, int total)> onSuccess,
    std::function<void(std::string)>                          onError)
{
    // Escape single quotes in query.
    std::string q;
    for (char c : query) { if (c == '\'') q += "''"; else q += c; }
    std::string where = q.empty()
        ? "TRUE"
        : "(name ILIKE '%" + q + "%' OR brand ILIKE '%" + q + "%')";

    std::string countSql = "SELECT COUNT(*) FROM foods WHERE " + where;
    std::string dataSql  =
        "SELECT id, name, brand, barcode, serving_size_g, calories, protein_g, carbs_g, fat_g,"
        " fiber_g, sugar_g, sodium_mg, created_by_user_id, is_verified,"
        " created_at::text, updated_at::text"
        " FROM foods WHERE " + where +
        " ORDER BY name LIMIT " + std::to_string(limit) +
        " OFFSET " + std::to_string(offset);

    auto self        = shared_from_this();
    auto onSuccessPtr = std::make_shared<decltype(onSuccess)>(std::move(onSuccess));
    auto onErrorPtr   = std::make_shared<decltype(onError)>(std::move(onError));

    db_->execSqlAsync(
        countSql,
        [self, dataSql, onSuccessPtr, onErrorPtr](const drogon::orm::Result& r) {
            int total = r[0][0].as<int>();
            self->db_->execSqlAsync(
                dataSql,
                [onSuccessPtr, total](const drogon::orm::Result& r2) {
                    std::vector<domain::Food> foods;
                    foods.reserve(r2.size());
                    for (const auto& row : r2) foods.push_back(rowToFood(row));
                    (*onSuccessPtr)(std::move(foods), total);
                },
                [onErrorPtr](const drogon::orm::DrogonDbException& e) {
                    (*onErrorPtr)(e.base().what());
                });
        },
        [onErrorPtr](const drogon::orm::DrogonDbException& e) {
            (*onErrorPtr)(e.base().what());
        });
}

void PostgresFoodRepository::findById(
    int64_t id,
    std::function<void(std::optional<domain::Food>)> onSuccess,
    std::function<void(std::string)>                 onError)
{
    auto self = shared_from_this();
    db_->execSqlAsync(
        "SELECT id, name, brand, barcode, serving_size_g, calories, protein_g, carbs_g, fat_g,"
        " fiber_g, sugar_g, sodium_mg, created_by_user_id, is_verified,"
        " created_at::text, updated_at::text FROM foods WHERE id = $1",
        [self, onSuccess = std::move(onSuccess)](const drogon::orm::Result& r) {
            if (r.empty()) { onSuccess(std::nullopt); return; }
            onSuccess(rowToFood(r[0]));
        },
        [onError = std::move(onError)](const drogon::orm::DrogonDbException& e) {
            onError(e.base().what());
        },
        id);
}

void PostgresFoodRepository::create(
    domain::Food food,
    std::function<void(domain::Food)>    onSuccess,
    std::function<void(std::string)>     onError)
{
    std::string brandSql   = food.brand.empty()   ? "NULL" : ("'" + food.brand   + "'");
    std::string barcodeSql = food.barcode.empty() ? "NULL" : ("'" + food.barcode + "'");
    std::string ownerSql   = food.created_by_user_id == 0
                             ? "NULL" : std::to_string(food.created_by_user_id);

    std::string sql =
        "INSERT INTO foods"
        " (name, brand, barcode, serving_size_g, calories, protein_g, carbs_g, fat_g,"
        "  fiber_g, sugar_g, sodium_mg, created_by_user_id, is_verified)"
        " VALUES ($1, " + brandSql + ", " + barcodeSql + ", $2, $3, $4, $5, $6, $7, $8, $9,"
        "  " + ownerSql + ", " + (food.is_verified ? "TRUE" : "FALSE") + ")"
        " RETURNING id, name, brand, barcode, serving_size_g, calories, protein_g, carbs_g,"
        "  fat_g, fiber_g, sugar_g, sodium_mg, created_by_user_id, is_verified,"
        "  created_at::text, updated_at::text";

    auto self = shared_from_this();
    db_->execSqlAsync(
        sql,
        [self, onSuccess = std::move(onSuccess)](const drogon::orm::Result& r) {
            onSuccess(rowToFood(r[0]));
        },
        [onError = std::move(onError)](const drogon::orm::DrogonDbException& e) {
            onError(e.base().what());
        },
        food.name, food.serving_size_g, food.calories, food.protein_g,
        food.carbs_g, food.fat_g, food.fiber_g, food.sugar_g, food.sodium_mg);
}

void PostgresFoodRepository::update(
    domain::Food food,
    std::function<void(domain::Food)>    onSuccess,
    std::function<void(std::string)>     onError)
{
    std::string brandSql   = food.brand.empty()   ? "NULL" : ("'" + food.brand   + "'");
    std::string barcodeSql = food.barcode.empty() ? "NULL" : ("'" + food.barcode + "'");

    std::string sql =
        "UPDATE foods SET name = $1, brand = " + brandSql + ", barcode = " + barcodeSql + ","
        " serving_size_g = $2, calories = $3, protein_g = $4, carbs_g = $5, fat_g = $6,"
        " fiber_g = $7, sugar_g = $8, sodium_mg = $9, updated_at = NOW()"
        " WHERE id = $10"
        " RETURNING id, name, brand, barcode, serving_size_g, calories, protein_g, carbs_g,"
        "  fat_g, fiber_g, sugar_g, sodium_mg, created_by_user_id, is_verified,"
        "  created_at::text, updated_at::text";

    auto self = shared_from_this();
    db_->execSqlAsync(
        sql,
        [self, onSuccess = std::move(onSuccess), onError]
        (const drogon::orm::Result& r) {
            if (r.empty()) { onError("Food not found"); return; }
            onSuccess(rowToFood(r[0]));
        },
        [onError = std::move(onError)](const drogon::orm::DrogonDbException& e) {
            onError(e.base().what());
        },
        food.name, food.serving_size_g, food.calories, food.protein_g,
        food.carbs_g, food.fat_g, food.fiber_g, food.sugar_g, food.sodium_mg, food.id);
}

void PostgresFoodRepository::remove(
    int64_t id, int64_t userId,
    std::function<void()>            onSuccess,
    std::function<void(std::string)> onError)
{
    auto self = shared_from_this();
    db_->execSqlAsync(
        "DELETE FROM foods WHERE id = $1 AND created_by_user_id = $2",
        [self, onSuccess = std::move(onSuccess)](const drogon::orm::Result&) {
            onSuccess();
        },
        [onError = std::move(onError)](const drogon::orm::DrogonDbException& e) {
            onError(e.base().what());
        },
        id, userId);
}

} // namespace infrastructure
