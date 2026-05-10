#include <drogon/drogon.h>

// ── Infrastructure ────────────────────────────────────────────────────────────
#include "infrastructure/loaders/JsonCompendiumLoader.hpp"
#include "infrastructure/auth/PasswordHasher.hpp"
#include "infrastructure/auth/TokenGenerator.hpp"
#include "infrastructure/persistence/PostgresUserRepository.hpp"
#include "infrastructure/persistence/PostgresSessionRepository.hpp"
#include "infrastructure/persistence/PostgresUserProfileRepository.hpp"
#include "infrastructure/persistence/PostgresUserGoalRepository.hpp"
#include "infrastructure/persistence/PostgresWeightLogRepository.hpp"
#include "infrastructure/persistence/PostgresBodyMeasurementRepository.hpp"
#include "infrastructure/persistence/PostgresFoodRepository.hpp"
#include "infrastructure/persistence/PostgresDailyLogRepository.hpp"
#include "infrastructure/persistence/PostgresMealEntryRepository.hpp"
#include "infrastructure/persistence/PostgresExerciseRepository.hpp"
#include "infrastructure/persistence/PostgresExerciseLogRepository.hpp"

// ── Application use cases ─────────────────────────────────────────────────────
#include "application/use_cases/SeedExercisesUseCase.hpp"
#include "application/use_cases/auth/RegisterUseCase.hpp"
#include "application/use_cases/auth/LoginUseCase.hpp"
#include "application/use_cases/auth/LogoutUseCase.hpp"
#include "application/use_cases/auth/GetCurrentUserUseCase.hpp"
#include "application/use_cases/profile/GetProfileUseCase.hpp"
#include "application/use_cases/profile/UpsertProfileUseCase.hpp"
#include "application/use_cases/goals/GetActiveGoalUseCase.hpp"
#include "application/use_cases/goals/CreateGoalUseCase.hpp"
#include "application/use_cases/goals/UpdateGoalUseCase.hpp"
#include "application/use_cases/goals/DeactivateGoalUseCase.hpp"
#include "application/use_cases/weight/ListWeightLogsUseCase.hpp"
#include "application/use_cases/weight/CreateWeightLogUseCase.hpp"
#include "application/use_cases/weight/UpdateWeightLogUseCase.hpp"
#include "application/use_cases/weight/DeleteWeightLogUseCase.hpp"
#include "application/use_cases/measurements/ListMeasurementsUseCase.hpp"
#include "application/use_cases/measurements/CreateMeasurementUseCase.hpp"
#include "application/use_cases/measurements/DeleteMeasurementUseCase.hpp"
#include "application/use_cases/foods/SearchFoodsUseCase.hpp"
#include "application/use_cases/foods/GetFoodUseCase.hpp"
#include "application/use_cases/foods/CreateFoodUseCase.hpp"
#include "application/use_cases/foods/UpdateFoodUseCase.hpp"
#include "application/use_cases/foods/DeleteFoodUseCase.hpp"
#include "application/use_cases/daily/GetDailyLogUseCase.hpp"
#include "application/use_cases/daily/UpdateDailyLogUseCase.hpp"
#include "application/use_cases/daily/AddMealEntryUseCase.hpp"
#include "application/use_cases/daily/UpdateMealEntryUseCase.hpp"
#include "application/use_cases/daily/DeleteMealEntryUseCase.hpp"
#include "application/use_cases/exercises/SearchExercisesUseCase.hpp"
#include "application/use_cases/exercises/GetExerciseUseCase.hpp"
#include "application/use_cases/exercises/CreateExerciseUseCase.hpp"
#include "application/use_cases/exercise_logs/ListExerciseLogsUseCase.hpp"
#include "application/use_cases/exercise_logs/CreateExerciseLogUseCase.hpp"
#include "application/use_cases/exercise_logs/UpdateExerciseLogUseCase.hpp"
#include "application/use_cases/exercise_logs/DeleteExerciseLogUseCase.hpp"

// ── Presentation ──────────────────────────────────────────────────────────────
#include "presentation/middleware/AuthMiddleware.hpp"
#include "presentation/controllers/AdminController.hpp"
#include "presentation/controllers/AuthController.hpp"
#include "presentation/controllers/ProfileController.hpp"
#include "presentation/controllers/GoalController.hpp"
#include "presentation/controllers/WeightController.hpp"
#include "presentation/controllers/MeasurementController.hpp"
#include "presentation/controllers/FoodController.hpp"
#include "presentation/controllers/DailyLogController.hpp"
#include "presentation/controllers/ExerciseController.hpp"
#include "presentation/controllers/ExerciseLogController.hpp"

using namespace drogon;
using namespace infrastructure;
using namespace application;
using namespace presentation;

// Helper: wrap a handler lambda so the response callback is a shared_ptr.
// All handlers follow the pattern: (req, cbPtr, ...) where cbPtr = shared_ptr<RespCb>.
#define DB drogon::app().getDbClient()

// ── Protected route macro helpers ─────────────────────────────────────────────
// We use lambdas that capture per-request and create repos/use cases inline.

int main()
{
    // ── Database config ───────────────────────────────────────────────────────
    drogon::orm::PostgresConfig pgConfig;
    pgConfig.host             = "127.0.0.1";
    pgConfig.port             = 5432;
    pgConfig.databaseName     = "calorie_tracker";
    pgConfig.username         = "admin";
    pgConfig.password         = "secret";
    pgConfig.connectionNumber = 10;
    pgConfig.name             = "default";
    pgConfig.isFast           = false;
    pgConfig.timeout          = -1.0;
    pgConfig.autoBatch        = false;

    // Long-lived singletons (created before app starts).
    auto compendiumLoader = std::make_shared<JsonCompendiumLoader>(
        "data/pa_compendium_activities.json");
    auto hasher    = std::make_shared<PasswordHasher>();
    auto tokenGen  = std::make_shared<TokenGenerator>();

    // ── App builder ───────────────────────────────────────────────────────────
    drogon::app()
        .addListener("0.0.0.0", 8080)
        .setLogLevel(trantor::Logger::kInfo)
        .addDbClient(pgConfig)

        // GET /
        .registerHandler("/",
            [](const HttpRequestPtr&, std::function<void(const HttpResponsePtr&)>&& cb) {
                auto resp = HttpResponse::newHttpResponse();
                resp->setBody("Calorie Tracker API v1");
                cb(resp);
            })

        // ── POST /admin/seed-exercises ────────────────────────────────────────
        .registerHandler("/admin/seed-exercises",
            [compendiumLoader](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
                auto repo    = std::make_shared<PostgresExerciseRepository>(DB);
                auto useCase = std::make_shared<SeedExercisesUseCase>(compendiumLoader, repo);
                auto ctrl    = std::make_shared<AdminController>(useCase);
                ctrl->seedExercises(req, std::move(cb));
            }, {Post})

        // ── POST /auth/register ───────────────────────────────────────────────
        .registerHandler("/auth/register",
            [hasher, tokenGen](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
                auto cbPtr = std::make_shared<RespCb>(std::move(cb));
                auto useCase = std::make_shared<RegisterUseCase>(
                    std::make_shared<PostgresUserRepository>(DB),
                    std::make_shared<PostgresSessionRepository>(DB),
                    hasher, tokenGen);
                AuthController::registerUser(req, cbPtr, useCase);
            }, {Post})

        // ── POST /auth/login ──────────────────────────────────────────────────
        .registerHandler("/auth/login",
            [hasher, tokenGen](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
                auto cbPtr = std::make_shared<RespCb>(std::move(cb));
                auto useCase = std::make_shared<LoginUseCase>(
                    std::make_shared<PostgresUserRepository>(DB),
                    std::make_shared<PostgresSessionRepository>(DB),
                    hasher, tokenGen);
                AuthController::login(req, cbPtr, useCase);
            }, {Post})

        // ── POST /auth/logout ─────────────────────────────────────────────────
        .registerHandler("/auth/logout",
            [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
                auto cbPtr = std::make_shared<RespCb>(std::move(cb));
                AuthMiddleware::authenticate(req, cbPtr,
                    [req, cbPtr](int64_t, const std::string& token) {
                        auto useCase = std::make_shared<LogoutUseCase>(
                            std::make_shared<PostgresSessionRepository>(DB));
                        AuthController::logout(req, cbPtr, token, useCase);
                    });
            }, {Post})

        // ── GET /auth/me ──────────────────────────────────────────────────────
        .registerHandler("/auth/me",
            [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
                auto cbPtr = std::make_shared<RespCb>(std::move(cb));
                AuthMiddleware::authenticate(req, cbPtr,
                    [req, cbPtr](int64_t userId, const std::string&) {
                        auto useCase = std::make_shared<GetCurrentUserUseCase>(
                            std::make_shared<PostgresUserRepository>(DB));
                        AuthController::me(req, cbPtr, userId, useCase);
                    });
            }, {Get})

        // ── GET /profile ──────────────────────────────────────────────────────
        .registerHandler("/profile",
            [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
                auto cbPtr = std::make_shared<RespCb>(std::move(cb));
                AuthMiddleware::authenticate(req, cbPtr,
                    [req, cbPtr](int64_t userId, const std::string&) {
                        auto useCase = std::make_shared<GetProfileUseCase>(
                            std::make_shared<PostgresUserProfileRepository>(DB));
                        ProfileController::getProfile(req, cbPtr, userId, useCase);
                    });
            }, {Get})

        // ── PUT /profile ──────────────────────────────────────────────────────
        .registerHandler("/profile",
            [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
                auto cbPtr = std::make_shared<RespCb>(std::move(cb));
                AuthMiddleware::authenticate(req, cbPtr,
                    [req, cbPtr](int64_t userId, const std::string&) {
                        auto useCase = std::make_shared<UpsertProfileUseCase>(
                            std::make_shared<PostgresUserProfileRepository>(DB));
                        ProfileController::upsertProfile(req, cbPtr, userId, useCase);
                    });
            }, {Put})

        // ── GET /goals/active ─────────────────────────────────────────────────
        .registerHandler("/goals/active",
            [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
                auto cbPtr = std::make_shared<RespCb>(std::move(cb));
                AuthMiddleware::authenticate(req, cbPtr,
                    [req, cbPtr](int64_t userId, const std::string&) {
                        auto useCase = std::make_shared<GetActiveGoalUseCase>(
                            std::make_shared<PostgresUserGoalRepository>(DB));
                        GoalController::getActive(req, cbPtr, userId, useCase);
                    });
            }, {Get})

        // ── POST /goals ───────────────────────────────────────────────────────
        .registerHandler("/goals",
            [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
                auto cbPtr = std::make_shared<RespCb>(std::move(cb));
                AuthMiddleware::authenticate(req, cbPtr,
                    [req, cbPtr](int64_t userId, const std::string&) {
                        auto useCase = std::make_shared<CreateGoalUseCase>(
                            std::make_shared<PostgresUserGoalRepository>(DB));
                        GoalController::create(req, cbPtr, userId, useCase);
                    });
            }, {Post})

        // ── PUT /goals/{id} ───────────────────────────────────────────────────
        .registerHandler("/goals/{goal_id}",
            [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb, const std::string& goalIdStr) {
                auto cbPtr  = std::make_shared<RespCb>(std::move(cb));
                int64_t gid = std::stoll(goalIdStr);
                AuthMiddleware::authenticate(req, cbPtr,
                    [req, cbPtr, gid](int64_t userId, const std::string&) {
                        auto useCase = std::make_shared<UpdateGoalUseCase>(
                            std::make_shared<PostgresUserGoalRepository>(DB));
                        GoalController::update(req, cbPtr, gid, userId, useCase);
                    });
            }, {Put})

        // ── DELETE /goals/{id} ────────────────────────────────────────────────
        .registerHandler("/goals/{goal_id}",
            [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb, const std::string& goalIdStr) {
                auto cbPtr  = std::make_shared<RespCb>(std::move(cb));
                int64_t gid = std::stoll(goalIdStr);
                AuthMiddleware::authenticate(req, cbPtr,
                    [req, cbPtr, gid](int64_t userId, const std::string&) {
                        auto useCase = std::make_shared<DeactivateGoalUseCase>(
                            std::make_shared<PostgresUserGoalRepository>(DB));
                        GoalController::deactivate(req, cbPtr, gid, userId, useCase);
                    });
            }, {Delete})

        // ── GET /weight ───────────────────────────────────────────────────────
        .registerHandler("/weight",
            [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
                auto cbPtr = std::make_shared<RespCb>(std::move(cb));
                AuthMiddleware::authenticate(req, cbPtr,
                    [req, cbPtr](int64_t userId, const std::string&) {
                        auto useCase = std::make_shared<ListWeightLogsUseCase>(
                            std::make_shared<PostgresWeightLogRepository>(DB));
                        WeightController::list(req, cbPtr, userId, useCase);
                    });
            }, {Get})

        // ── POST /weight ──────────────────────────────────────────────────────
        .registerHandler("/weight",
            [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
                auto cbPtr = std::make_shared<RespCb>(std::move(cb));
                AuthMiddleware::authenticate(req, cbPtr,
                    [req, cbPtr](int64_t userId, const std::string&) {
                        auto useCase = std::make_shared<CreateWeightLogUseCase>(
                            std::make_shared<PostgresWeightLogRepository>(DB));
                        WeightController::create(req, cbPtr, userId, useCase);
                    });
            }, {Post})

        // ── PUT /weight/{id} ──────────────────────────────────────────────────
        .registerHandler("/weight/{log_id}",
            [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb, const std::string& idStr) {
                auto cbPtr = std::make_shared<RespCb>(std::move(cb));
                int64_t id = std::stoll(idStr);
                AuthMiddleware::authenticate(req, cbPtr,
                    [req, cbPtr, id](int64_t userId, const std::string&) {
                        auto useCase = std::make_shared<UpdateWeightLogUseCase>(
                            std::make_shared<PostgresWeightLogRepository>(DB));
                        WeightController::update(req, cbPtr, id, userId, useCase);
                    });
            }, {Put})

        // ── DELETE /weight/{id} ───────────────────────────────────────────────
        .registerHandler("/weight/{log_id}",
            [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb, const std::string& idStr) {
                auto cbPtr = std::make_shared<RespCb>(std::move(cb));
                int64_t id = std::stoll(idStr);
                AuthMiddleware::authenticate(req, cbPtr,
                    [req, cbPtr, id](int64_t userId, const std::string&) {
                        auto useCase = std::make_shared<DeleteWeightLogUseCase>(
                            std::make_shared<PostgresWeightLogRepository>(DB));
                        WeightController::remove(req, cbPtr, id, userId, useCase);
                    });
            }, {Delete})

        // ── GET /measurements ─────────────────────────────────────────────────
        .registerHandler("/measurements",
            [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
                auto cbPtr = std::make_shared<RespCb>(std::move(cb));
                AuthMiddleware::authenticate(req, cbPtr,
                    [req, cbPtr](int64_t userId, const std::string&) {
                        auto useCase = std::make_shared<ListMeasurementsUseCase>(
                            std::make_shared<PostgresBodyMeasurementRepository>(DB));
                        MeasurementController::list(req, cbPtr, userId, useCase);
                    });
            }, {Get})

        // ── POST /measurements ────────────────────────────────────────────────
        .registerHandler("/measurements",
            [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
                auto cbPtr = std::make_shared<RespCb>(std::move(cb));
                AuthMiddleware::authenticate(req, cbPtr,
                    [req, cbPtr](int64_t userId, const std::string&) {
                        auto useCase = std::make_shared<CreateMeasurementUseCase>(
                            std::make_shared<PostgresBodyMeasurementRepository>(DB),
                            std::make_shared<PostgresUserProfileRepository>(DB));
                        MeasurementController::create(req, cbPtr, userId, useCase);
                    });
            }, {Post})

        // ── DELETE /measurements/{id} ─────────────────────────────────────────
        .registerHandler("/measurements/{id}",
            [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb, const std::string& idStr) {
                auto cbPtr = std::make_shared<RespCb>(std::move(cb));
                int64_t id = std::stoll(idStr);
                AuthMiddleware::authenticate(req, cbPtr,
                    [req, cbPtr, id](int64_t userId, const std::string&) {
                        auto useCase = std::make_shared<DeleteMeasurementUseCase>(
                            std::make_shared<PostgresBodyMeasurementRepository>(DB));
                        MeasurementController::remove(req, cbPtr, id, userId, useCase);
                    });
            }, {Delete})

        // ── GET /foods ────────────────────────────────────────────────────────
        .registerHandler("/foods",
            [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
                auto cbPtr = std::make_shared<RespCb>(std::move(cb));
                AuthMiddleware::authenticate(req, cbPtr,
                    [req, cbPtr](int64_t, const std::string&) {
                        auto useCase = std::make_shared<SearchFoodsUseCase>(
                            std::make_shared<PostgresFoodRepository>(DB));
                        FoodController::search(req, cbPtr, useCase);
                    });
            }, {Get})

        // ── GET /foods/{id} ───────────────────────────────────────────────────
        .registerHandler("/foods/{food_id}",
            [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb, const std::string& idStr) {
                auto cbPtr = std::make_shared<RespCb>(std::move(cb));
                int64_t id = std::stoll(idStr);
                AuthMiddleware::authenticate(req, cbPtr,
                    [req, cbPtr, id](int64_t, const std::string&) {
                        auto useCase = std::make_shared<GetFoodUseCase>(
                            std::make_shared<PostgresFoodRepository>(DB));
                        FoodController::getOne(req, cbPtr, id, useCase);
                    });
            }, {Get})

        // ── POST /foods ───────────────────────────────────────────────────────
        .registerHandler("/foods",
            [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
                auto cbPtr = std::make_shared<RespCb>(std::move(cb));
                AuthMiddleware::authenticate(req, cbPtr,
                    [req, cbPtr](int64_t userId, const std::string&) {
                        auto useCase = std::make_shared<CreateFoodUseCase>(
                            std::make_shared<PostgresFoodRepository>(DB));
                        FoodController::create(req, cbPtr, userId, useCase);
                    });
            }, {Post})

        // ── PUT /foods/{id} ───────────────────────────────────────────────────
        .registerHandler("/foods/{food_id}",
            [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb, const std::string& idStr) {
                auto cbPtr = std::make_shared<RespCb>(std::move(cb));
                int64_t id = std::stoll(idStr);
                AuthMiddleware::authenticate(req, cbPtr,
                    [req, cbPtr, id](int64_t, const std::string&) {
                        auto useCase = std::make_shared<UpdateFoodUseCase>(
                            std::make_shared<PostgresFoodRepository>(DB));
                        FoodController::update(req, cbPtr, id, useCase);
                    });
            }, {Put})

        // ── DELETE /foods/{id} ────────────────────────────────────────────────
        .registerHandler("/foods/{food_id}",
            [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb, const std::string& idStr) {
                auto cbPtr = std::make_shared<RespCb>(std::move(cb));
                int64_t id = std::stoll(idStr);
                AuthMiddleware::authenticate(req, cbPtr,
                    [req, cbPtr, id](int64_t userId, const std::string&) {
                        auto useCase = std::make_shared<DeleteFoodUseCase>(
                            std::make_shared<PostgresFoodRepository>(DB));
                        FoodController::remove(req, cbPtr, id, userId, useCase);
                    });
            }, {Delete})

        // ── GET /daily ────────────────────────────────────────────────────────
        .registerHandler("/daily",
            [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
                auto cbPtr = std::make_shared<RespCb>(std::move(cb));
                AuthMiddleware::authenticate(req, cbPtr,
                    [req, cbPtr](int64_t userId, const std::string&) {
                        auto useCase = std::make_shared<GetDailyLogUseCase>(
                            std::make_shared<PostgresDailyLogRepository>(DB),
                            std::make_shared<PostgresMealEntryRepository>(DB));
                        DailyLogController::getByDate(req, cbPtr, userId, useCase);
                    });
            }, {Get})

        // ── PUT /daily/{id} ───────────────────────────────────────────────────
        .registerHandler("/daily/{log_id}",
            [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb, const std::string& idStr) {
                auto cbPtr = std::make_shared<RespCb>(std::move(cb));
                int64_t id = std::stoll(idStr);
                AuthMiddleware::authenticate(req, cbPtr,
                    [req, cbPtr, id](int64_t userId, const std::string&) {
                        auto useCase = std::make_shared<UpdateDailyLogUseCase>(
                            std::make_shared<PostgresDailyLogRepository>(DB));
                        DailyLogController::update(req, cbPtr, id, userId, useCase);
                    });
            }, {Put})

        // ── POST /daily/meals ─────────────────────────────────────────────────
        .registerHandler("/daily/meals",
            [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
                auto cbPtr = std::make_shared<RespCb>(std::move(cb));
                AuthMiddleware::authenticate(req, cbPtr,
                    [req, cbPtr](int64_t userId, const std::string&) {
                        auto useCase = std::make_shared<AddMealEntryUseCase>(
                            std::make_shared<PostgresDailyLogRepository>(DB),
                            std::make_shared<PostgresMealEntryRepository>(DB),
                            std::make_shared<PostgresFoodRepository>(DB));
                        DailyLogController::addMeal(req, cbPtr, userId, useCase);
                    });
            }, {Post})

        // ── PUT /daily/meals/{entry_id}/{log_id} ──────────────────────────────
        .registerHandler("/daily/meals/{entry_id}/{log_id}",
            [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb,
               const std::string& entryIdStr, const std::string& logIdStr) {
                auto cbPtr   = std::make_shared<RespCb>(std::move(cb));
                int64_t eid  = std::stoll(entryIdStr);
                int64_t lid  = std::stoll(logIdStr);
                AuthMiddleware::authenticate(req, cbPtr,
                    [req, cbPtr, eid, lid](int64_t, const std::string&) {
                        auto useCase = std::make_shared<UpdateMealEntryUseCase>(
                            std::make_shared<PostgresMealEntryRepository>(DB),
                            std::make_shared<PostgresFoodRepository>(DB));
                        DailyLogController::updateMeal(req, cbPtr, eid, lid, useCase);
                    });
            }, {Put})

        // ── DELETE /daily/meals/{entry_id}/{log_id} ───────────────────────────
        .registerHandler("/daily/meals/{entry_id}/{log_id}",
            [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb,
               const std::string& entryIdStr, const std::string& logIdStr) {
                auto cbPtr   = std::make_shared<RespCb>(std::move(cb));
                int64_t eid  = std::stoll(entryIdStr);
                int64_t lid  = std::stoll(logIdStr);
                AuthMiddleware::authenticate(req, cbPtr,
                    [req, cbPtr, eid, lid](int64_t, const std::string&) {
                        auto useCase = std::make_shared<DeleteMealEntryUseCase>(
                            std::make_shared<PostgresMealEntryRepository>(DB));
                        DailyLogController::deleteMeal(req, cbPtr, eid, lid, useCase);
                    });
            }, {Delete})

        // ── GET /exercises ────────────────────────────────────────────────────
        .registerHandler("/exercises",
            [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
                auto cbPtr = std::make_shared<RespCb>(std::move(cb));
                AuthMiddleware::authenticate(req, cbPtr,
                    [req, cbPtr](int64_t, const std::string&) {
                        auto useCase = std::make_shared<SearchExercisesUseCase>(
                            std::make_shared<PostgresExerciseRepository>(DB));
                        ExerciseController::search(req, cbPtr, useCase);
                    });
            }, {Get})

        // ── GET /exercises/{id} ───────────────────────────────────────────────
        .registerHandler("/exercises/{ex_id}",
            [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb, const std::string& idStr) {
                auto cbPtr = std::make_shared<RespCb>(std::move(cb));
                int64_t id = std::stoll(idStr);
                AuthMiddleware::authenticate(req, cbPtr,
                    [req, cbPtr, id](int64_t, const std::string&) {
                        auto useCase = std::make_shared<GetExerciseUseCase>(
                            std::make_shared<PostgresExerciseRepository>(DB));
                        ExerciseController::getOne(req, cbPtr, id, useCase);
                    });
            }, {Get})

        // ── POST /exercises ───────────────────────────────────────────────────
        .registerHandler("/exercises",
            [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
                auto cbPtr = std::make_shared<RespCb>(std::move(cb));
                AuthMiddleware::authenticate(req, cbPtr,
                    [req, cbPtr](int64_t userId, const std::string&) {
                        auto useCase = std::make_shared<CreateExerciseUseCase>(
                            std::make_shared<PostgresExerciseRepository>(DB));
                        ExerciseController::create(req, cbPtr, userId, useCase);
                    });
            }, {Post})

        // ── GET /exercise-logs ────────────────────────────────────────────────
        .registerHandler("/exercise-logs",
            [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
                auto cbPtr = std::make_shared<RespCb>(std::move(cb));
                AuthMiddleware::authenticate(req, cbPtr,
                    [req, cbPtr](int64_t userId, const std::string&) {
                        auto useCase = std::make_shared<ListExerciseLogsUseCase>(
                            std::make_shared<PostgresExerciseLogRepository>(DB));
                        ExerciseLogController::list(req, cbPtr, userId, useCase);
                    });
            }, {Get})

        // ── POST /exercise-logs ───────────────────────────────────────────────
        .registerHandler("/exercise-logs",
            [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
                auto cbPtr = std::make_shared<RespCb>(std::move(cb));
                AuthMiddleware::authenticate(req, cbPtr,
                    [req, cbPtr](int64_t userId, const std::string&) {
                        auto useCase = std::make_shared<CreateExerciseLogUseCase>(
                            std::make_shared<PostgresExerciseLogRepository>(DB),
                            std::make_shared<PostgresExerciseRepository>(DB),
                            std::make_shared<PostgresWeightLogRepository>(DB));
                        ExerciseLogController::create(req, cbPtr, userId, useCase);
                    });
            }, {Post})

        // ── PUT /exercise-logs/{id} ───────────────────────────────────────────
        .registerHandler("/exercise-logs/{log_id}",
            [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb, const std::string& idStr) {
                auto cbPtr = std::make_shared<RespCb>(std::move(cb));
                int64_t id = std::stoll(idStr);
                AuthMiddleware::authenticate(req, cbPtr,
                    [req, cbPtr, id](int64_t userId, const std::string&) {
                        auto useCase = std::make_shared<UpdateExerciseLogUseCase>(
                            std::make_shared<PostgresExerciseLogRepository>(DB));
                        ExerciseLogController::update(req, cbPtr, id, userId, useCase);
                    });
            }, {Put})

        // ── DELETE /exercise-logs/{id} ────────────────────────────────────────
        .registerHandler("/exercise-logs/{log_id}",
            [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb, const std::string& idStr) {
                auto cbPtr = std::make_shared<RespCb>(std::move(cb));
                int64_t id = std::stoll(idStr);
                AuthMiddleware::authenticate(req, cbPtr,
                    [req, cbPtr, id](int64_t userId, const std::string&) {
                        auto useCase = std::make_shared<DeleteExerciseLogUseCase>(
                            std::make_shared<PostgresExerciseLogRepository>(DB));
                        ExerciseLogController::remove(req, cbPtr, id, userId, useCase);
                    });
            }, {Delete})

        .run();

    return 0;
}
