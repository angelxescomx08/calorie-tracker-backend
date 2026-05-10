#include "infrastructure/loaders/JsonCompendiumLoader.hpp"
#include <fstream>
#include <json/json.h>

namespace infrastructure {

JsonCompendiumLoader::JsonCompendiumLoader(std::string filePath)
    : filePath_(std::move(filePath))
{}

bool JsonCompendiumLoader::load(
    std::vector<domain::Exercise>& out,
    std::string&                   errorOut
)
{
    std::ifstream file(filePath_);
    if (!file.is_open()) {
        errorOut = "Cannot open: " + filePath_;
        return false;
    }

    Json::Value             root;
    Json::CharReaderBuilder builder;
    if (!Json::parseFromStream(builder, file, &root, &errorOut)) {
        errorOut = "JSON parse error: " + errorOut;
        return false;
    }

    out.clear();
    out.reserve(root["total_activities"].asInt());

    for (const auto& cat : root["categories"]) {
        const std::string prefix   = cat["code_prefix"].asString();
        const std::string catName  = cat["name"].asString();
        const auto        category = mapCategory(prefix);

        for (const auto& act : cat["activities"]) {
            const std::string desc = act["description"].asString();

            domain::Exercise ex;
            ex.name        = desc.size() > 255 ? desc.substr(0, 255) : desc;
            ex.category    = category;
            ex.met_value   = act["met_value"].asDouble();
            ex.description = "[" + catName + "] " + desc;
            ex.is_verified = true;
            out.push_back(std::move(ex));
        }
    }

    return true;
}

domain::ExerciseCategory JsonCompendiumLoader::mapCategory(const std::string& prefix)
{
    if (prefix == "01" || prefix == "03" || prefix == "12" ||
        prefix == "17" || prefix == "18")
        return domain::ExerciseCategory::Cardio;
    if (prefix == "02")
        return domain::ExerciseCategory::Strength;
    if (prefix == "15" || prefix == "19")
        return domain::ExerciseCategory::Sports;
    return domain::ExerciseCategory::Other;
}

} // namespace infrastructure
