cmake_minimum_required(VERSION 3.15)
project(calorie-tracker-backend CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Drogon CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)

# Drogon and Trantor hardcode MSVC-only warning flags; strip them for GCC/Clang
if(NOT MSVC)
    set_property(TARGET Drogon::Drogon   PROPERTY INTERFACE_COMPILE_OPTIONS "")
    set_property(TARGET Trantor::Trantor PROPERTY INTERFACE_COMPILE_OPTIONS "")
endif()

set(SOURCES
    main.cpp

    # ── Application ────────────────────────────────────────────
    src/application/use_cases/SeedExercisesUseCase.cpp

    # ── Infrastructure: auth ───────────────────────────────────
    src/infrastructure/auth/PasswordHasher.cpp
    src/infrastructure/auth/TokenGenerator.cpp

    # ── Infrastructure: loaders ────────────────────────────────
    src/infrastructure/loaders/JsonCompendiumLoader.cpp

    # ── Infrastructure: persistence ────────────────────────────
    src/infrastructure/persistence/PostgresUserRepository.cpp
    src/infrastructure/persistence/PostgresSessionRepository.cpp
    src/infrastructure/persistence/PostgresUserProfileRepository.cpp
    src/infrastructure/persistence/PostgresUserGoalRepository.cpp
    src/infrastructure/persistence/PostgresWeightLogRepository.cpp
    src/infrastructure/persistence/PostgresBodyMeasurementRepository.cpp
    src/infrastructure/persistence/PostgresFoodRepository.cpp
    src/infrastructure/persistence/PostgresDailyLogRepository.cpp
    src/infrastructure/persistence/PostgresMealEntryRepository.cpp
    src/infrastructure/persistence/PostgresExerciseRepository.cpp
    src/infrastructure/persistence/PostgresExerciseLogRepository.cpp

    # ── Presentation: middleware ───────────────────────────────
    src/presentation/middleware/AuthMiddleware.cpp

    # ── Presentation: controllers ──────────────────────────────
    src/presentation/controllers/AdminController.cpp
    src/presentation/controllers/AuthController.cpp
    src/presentation/controllers/ProfileController.cpp
    src/presentation/controllers/GoalController.cpp
    src/presentation/controllers/WeightController.cpp
    src/presentation/controllers/MeasurementController.cpp
    src/presentation/controllers/FoodController.cpp
    src/presentation/controllers/DailyLogController.cpp
    src/presentation/controllers/ExerciseController.cpp
    src/presentation/controllers/ExerciseLogController.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Drogon::Drogon
    OpenSSL::SSL
    OpenSSL::Crypto
)
