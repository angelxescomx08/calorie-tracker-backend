cmake_minimum_required(VERSION 3.15)
project(calorie-tracker-backend CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Drogon CONFIG REQUIRED)

# Drogon and Trantor hardcode MSVC-only warning flags; strip them for GCC/Clang
if(NOT MSVC)
    set_property(TARGET Drogon::Drogon   PROPERTY INTERFACE_COMPILE_OPTIONS "")
    set_property(TARGET Trantor::Trantor PROPERTY INTERFACE_COMPILE_OPTIONS "")
endif()

set(SOURCES
    main.cpp
    src/application/use_cases/SeedExercisesUseCase.cpp
    src/infrastructure/loaders/JsonCompendiumLoader.cpp
    src/infrastructure/persistence/PostgresExerciseRepository.cpp
    src/presentation/controllers/AdminController.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE Drogon::Drogon)
