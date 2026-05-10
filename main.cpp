#include <drogon/drogon.h>
#include <fstream>
#include <sstream>

static std::string sql_escape(const std::string& s) {
    std::string out;
    out.reserve(s.size() + 8);
    for (char c : s) {
        if (c == '\'') out += "''";
        else out += c;
    }
    return out;
}

static std::string map_category(const std::string& prefix) {
    if (prefix == "01" || prefix == "03" || prefix == "12" ||
        prefix == "17" || prefix == "18") return "cardio";
    if (prefix == "02") return "strength";
    if (prefix == "15" || prefix == "19") return "sports";
    return "other";
}

int main() {
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

    drogon::app()
        .addListener("0.0.0.0", 8080)
        .setLogLevel(trantor::Logger::kInfo)
        .addDbClient(pgConfig)

        // GET /
        .registerHandler("/",
            [](const drogon::HttpRequestPtr&,
               std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
                auto resp = drogon::HttpResponse::newHttpResponse();
                resp->setStatusCode(drogon::k200OK);
                resp->setContentTypeCode(drogon::CT_TEXT_PLAIN);
                resp->setBody("Hello, World!");
                callback(resp);
            })

        // POST /admin/seed-exercises
        .registerHandler("/admin/seed-exercises",
            [](const drogon::HttpRequestPtr&,
               std::function<void(const drogon::HttpResponsePtr&)>&& callback) {

                // Leer JSON del Compendium
                std::ifstream file("data/pa_compendium_activities.json");
                if (!file.is_open()) {
                    Json::Value body;
                    body["error"] = "Cannot open data/pa_compendium_activities.json";
                    auto resp = drogon::HttpResponse::newHttpJsonResponse(body);
                    resp->setStatusCode(drogon::k500InternalServerError);
                    callback(resp);
                    return;
                }

                Json::Value root;
                Json::CharReaderBuilder builder;
                std::string errors;
                if (!Json::parseFromStream(builder, file, &root, &errors)) {
                    Json::Value body;
                    body["error"] = "JSON parse error: " + errors;
                    auto resp = drogon::HttpResponse::newHttpJsonResponse(body);
                    resp->setStatusCode(drogon::k500InternalServerError);
                    callback(resp);
                    return;
                }

                // Construir INSERT masivo
                std::ostringstream sql;
                sql << "INSERT INTO exercises "
                       "(name, category, met_value, description, is_verified) VALUES ";

                bool first = true;
                int  count = 0;

                for (const auto& cat : root["categories"]) {
                    const std::string prefix   = cat["code_prefix"].asString();
                    const std::string category = map_category(prefix);
                    const std::string cat_name = cat["name"].asString();

                    for (const auto& act : cat["activities"]) {
                        const std::string desc = act["description"].asString();
                        const double      met  = act["met_value"].asDouble();
                        const std::string name = desc.size() > 255
                                                     ? desc.substr(0, 255)
                                                     : desc;
                        const std::string full = "[" + cat_name + "] " + desc;

                        if (!first) sql << ',';
                        sql << "('"  << sql_escape(name) << "',"
                            << "'"  << category          << "',"
                            << met  << ","
                            << "'"  << sql_escape(full)  << "',"
                            << "TRUE)";
                        first = false;
                        ++count;
                    }
                }

                auto db = drogon::app().getDbClient();
                db->execSqlAsync(
                    sql.str(),
                    [callback, count](const drogon::orm::Result&) {
                        Json::Value body;
                        body["inserted"] = count;
                        body["message"]  = "Exercises seeded successfully";
                        callback(drogon::HttpResponse::newHttpJsonResponse(body));
                    },
                    [callback](const drogon::orm::DrogonDbException& e) {
                        Json::Value body;
                        body["error"] = std::string(e.base().what());
                        auto resp = drogon::HttpResponse::newHttpJsonResponse(body);
                        resp->setStatusCode(drogon::k500InternalServerError);
                        callback(resp);
                    });
            },
            {drogon::Post})

        .run();
    return 0;
}
