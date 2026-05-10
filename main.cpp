#include <drogon/drogon.h>
#include "application/use_cases/SeedExercisesUseCase.hpp"
#include "infrastructure/loaders/JsonCompendiumLoader.hpp"
#include "infrastructure/persistence/PostgresExerciseRepository.hpp"
#include "presentation/controllers/AdminController.hpp"

int main()
{
    // ── Database config ───────────────────────────────────────────
    drogon::orm::PostgresConfig pgConfig;
    pgConfig.host             = "127.0.0.1";
    pgConfig.port             = 5432;
    pgConfig.databaseName     = "calorie_tracker";
    pgConfig.username         = "admin";
    pgConfig.password         = "secret";
    pgConfig.connectionNumber = 5;
    pgConfig.name             = "default";
    pgConfig.isFast           = false;
    pgConfig.timeout          = -1.0;
    pgConfig.autoBatch        = false;

    // ── Composition root ──────────────────────────────────────────
    // Long-lived loaders are created before run(); repositories and
    // controllers are created per-request so getDbClient() is always valid.

    auto compendiumLoader = std::make_shared<infrastructure::JsonCompendiumLoader>(
        "data/pa_compendium_activities.json"
    );

    // ── Routes ────────────────────────────────────────────────────
    drogon::app()
        .addListener("0.0.0.0", 8080)
        .setLogLevel(trantor::Logger::kInfo)
        .addDbClient(pgConfig)

        // GET /
        .registerHandler("/",
            [](const drogon::HttpRequestPtr&,
               std::function<void(const drogon::HttpResponsePtr&)>&& cb) {
                auto resp = drogon::HttpResponse::newHttpResponse();
                resp->setContentTypeCode(drogon::CT_TEXT_PLAIN);
                resp->setBody("Hello, World!");
                cb(resp);
            })

        // POST /admin/seed-exercises
        .registerHandler("/admin/seed-exercises",
            [compendiumLoader](
                const drogon::HttpRequestPtr& req,
                std::function<void(const drogon::HttpResponsePtr&)>&& cb
            ) {
                auto repo = std::make_shared<infrastructure::PostgresExerciseRepository>(
                    drogon::app().getDbClient()
                );
                auto useCase = std::make_shared<application::SeedExercisesUseCase>(
                    compendiumLoader, repo
                );
                auto ctrl = std::make_shared<presentation::AdminController>(useCase);
                ctrl->seedExercises(req, std::move(cb));
            },
            {drogon::Post})

        .run();

    return 0;
}
