#include <drogon/drogon.h>

int main() {
    drogon::app()
        .addListener("0.0.0.0", 8080)
        .setLogLevel(trantor::Logger::kInfo)
        .registerHandler("/",
            [](const drogon::HttpRequestPtr& req,
               std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
                auto resp = drogon::HttpResponse::newHttpResponse();
                resp->setStatusCode(drogon::k200OK);
                resp->setContentTypeCode(drogon::CT_TEXT_PLAIN);
                resp->setBody("Hello, World!");
                callback(resp);
            })
        .run();
    return 0;
}
