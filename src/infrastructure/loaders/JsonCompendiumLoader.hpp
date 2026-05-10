#pragma once
#include <string>
#include "application/ports/IExerciseLoader.hpp"

namespace infrastructure {

// Adapter: maps a PA Compendium JSON file to IExerciseLoader.
class JsonCompendiumLoader : public application::IExerciseLoader {
public:
    explicit JsonCompendiumLoader(std::string filePath);

    bool load(
        std::vector<domain::Exercise>& out,
        std::string&                   errorOut
    ) override;

private:
    std::string filePath_;

    static domain::ExerciseCategory mapCategory(const std::string& codePrefix);
};

} // namespace infrastructure
