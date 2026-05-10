#include "infrastructure/persistence/PostgresMealEntryRepository.hpp"

namespace infrastructure {

PostgresMealEntryRepository::PostgresMealEntryRepository(drogon::orm::DbClientPtr db)
    : db_(std::move(db)) {}

domain::MealEntry PostgresMealEntryRepository::rowToEntry(const drogon::orm::Row& row)
{
    domain::MealEntry m;
    m.id           = row["id"].as<int64_t>();
    m.daily_log_id = row["daily_log_id"].as<int64_t>();
    m.food_id      = row["food_id"].as<int64_t>();
    m.quantity_g   = row["quantity_g"].as<double>();
    m.calories     = row["calories"].as<double>();
    m.protein_g    = row["protein_g"].as<double>();
    m.carbs_g      = row["carbs_g"].as<double>();
    m.fat_g        = row["fat_g"].as<double>();
    m.created_at   = row["created_at"].as<std::string>();

    auto mt = row["meal_type"].as<std::string>();
    if      (mt == "breakfast") m.meal_type = domain::MealType::Breakfast;
    else if (mt == "lunch")     m.meal_type = domain::MealType::Lunch;
    else if (mt == "dinner")    m.meal_type = domain::MealType::Dinner;
    else                        m.meal_type = domain::MealType::Snack;

    return m;
}

static const char* mealTypeStr(domain::MealType mt) {
    switch (mt) {
        case domain::MealType::Breakfast: return "breakfast";
        case domain::MealType::Lunch:     return "lunch";
        case domain::MealType::Dinner:    return "dinner";
        default:                          return "snack";
    }
}

void PostgresMealEntryRepository::listByDailyLog(
    int64_t dailyLogId,
    std::function<void(std::vector<domain::MealEntry>)> onSuccess,
    std::function<void(std::string)>                    onError)
{
    auto self = shared_from_this();
    db_->execSqlAsync(
        "SELECT id, daily_log_id, meal_type, food_id, quantity_g, calories, protein_g,"
        " carbs_g, fat_g, created_at::text FROM meal_entries WHERE daily_log_id = $1"
        " ORDER BY created_at",
        [self, onSuccess = std::move(onSuccess)](const drogon::orm::Result& r) {
            std::vector<domain::MealEntry> entries;
            entries.reserve(r.size());
            for (const auto& row : r) entries.push_back(rowToEntry(row));
            onSuccess(std::move(entries));
        },
        [onError = std::move(onError)](const drogon::orm::DrogonDbException& e) {
            onError(e.base().what());
        },
        dailyLogId);
}

void PostgresMealEntryRepository::create(
    domain::MealEntry entry,
    std::function<void(domain::MealEntry)>   onSuccess,
    std::function<void(std::string)>         onError)
{
    auto self = shared_from_this();
    db_->execSqlAsync(
        "INSERT INTO meal_entries"
        " (daily_log_id, meal_type, food_id, quantity_g, calories, protein_g, carbs_g, fat_g)"
        " VALUES ($1, $2::meal_type, $3, $4, $5, $6, $7, $8)"
        " RETURNING id, daily_log_id, meal_type, food_id, quantity_g, calories, protein_g,"
        "  carbs_g, fat_g, created_at::text",
        [self, onSuccess = std::move(onSuccess)](const drogon::orm::Result& r) {
            onSuccess(rowToEntry(r[0]));
        },
        [onError = std::move(onError)](const drogon::orm::DrogonDbException& e) {
            onError(e.base().what());
        },
        entry.daily_log_id, std::string(mealTypeStr(entry.meal_type)), entry.food_id,
        entry.quantity_g, entry.calories, entry.protein_g, entry.carbs_g, entry.fat_g);
}

void PostgresMealEntryRepository::update(
    domain::MealEntry entry,
    std::function<void(domain::MealEntry)>   onSuccess,
    std::function<void(std::string)>         onError)
{
    auto self = shared_from_this();
    db_->execSqlAsync(
        "UPDATE meal_entries SET meal_type = $1::meal_type, food_id = $2, quantity_g = $3,"
        " calories = $4, protein_g = $5, carbs_g = $6, fat_g = $7"
        " WHERE id = $8 AND daily_log_id = $9"
        " RETURNING id, daily_log_id, meal_type, food_id, quantity_g, calories, protein_g,"
        "  carbs_g, fat_g, created_at::text",
        [self, onSuccess = std::move(onSuccess), onError]
        (const drogon::orm::Result& r) {
            if (r.empty()) { onError("Meal entry not found"); return; }
            onSuccess(rowToEntry(r[0]));
        },
        [onError = std::move(onError)](const drogon::orm::DrogonDbException& e) {
            onError(e.base().what());
        },
        std::string(mealTypeStr(entry.meal_type)), entry.food_id, entry.quantity_g,
        entry.calories, entry.protein_g, entry.carbs_g, entry.fat_g,
        entry.id, entry.daily_log_id);
}

void PostgresMealEntryRepository::remove(
    int64_t id, int64_t dailyLogId,
    std::function<void()>            onSuccess,
    std::function<void(std::string)> onError)
{
    auto self = shared_from_this();
    db_->execSqlAsync(
        "DELETE FROM meal_entries WHERE id = $1 AND daily_log_id = $2",
        [self, onSuccess = std::move(onSuccess)](const drogon::orm::Result&) {
            onSuccess();
        },
        [onError = std::move(onError)](const drogon::orm::DrogonDbException& e) {
            onError(e.base().what());
        },
        id, dailyLogId);
}

} // namespace infrastructure
