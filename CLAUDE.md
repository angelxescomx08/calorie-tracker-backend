# Calorie Tracker Backend — Coding Rules

## Stack
C++17 · Drogon (async HTTP) · PostgreSQL 17 · JsonCpp · CMake + vcpkg (x64-windows)

---

## Architecture: Clean Architecture + Adapter Pattern

### Layers and their directories

| Layer | Directory | Allowed dependencies |
|---|---|---|
| **Domain** | `src/domain/` | C++ stdlib only |
| **Application** | `src/application/` | Domain only |
| **Infrastructure** | `src/infrastructure/` | Domain + Application + external libs |
| **Presentation** | `src/presentation/` | Application + Drogon types |
| **Composition root** | `main.cpp` | All layers |

**The dependency rule: arrows point inward.**
`Presentation → Application → Domain ← Infrastructure`

Domain never includes Drogon, JsonCpp, or libpq headers. Application never includes them either. Violations of this rule are bugs, not style issues.

### Where each concept lives

```
src/domain/entities/          ← Plain data structs (Exercise, User, Food…)
src/domain/enums/             ← Domain enums (ExerciseCategory…)
src/domain/repositories/      ← Abstract repository interfaces (IExerciseRepository…)
src/application/ports/        ← Abstract service interfaces (IExerciseLoader…)
src/application/use_cases/    ← One use case per file (SeedExercisesUseCase…)
src/infrastructure/loaders/   ← File / external data adapters (JsonCompendiumLoader…)
src/infrastructure/persistence/ ← DB repository adapters (PostgresExerciseRepository…)
src/presentation/controllers/ ← HTTP ↔ use case translation (AdminController…)
```

### Adapter pattern rule
Every external system (PostgreSQL, JSON files, USDA API…) has **one adapter class** in `src/infrastructure/`. Adapters implement a domain or application interface. The rest of the code never imports Drogon ORM or JsonCpp types directly.

---

## SOLID rules for this project

**S — Single Responsibility**
- Controllers only translate HTTP ↔ use case calls. No business logic.
- Use cases only orchestrate. No SQL, no JSON parsing, no HTTP concepts.
- Repositories only persist. No calorie calculations, no HTTP.

**O — Open / Closed**
Add new behavior by creating new classes, not by modifying existing ones.
- New data source? Add `CsvCompendiumLoader : IExerciseLoader`.
- New database? Add `SqliteExerciseRepository : IExerciseRepository`.

**L — Liskov Substitution**
Any concrete `IExerciseRepository` must be substitutable for any other. Use cases must not downcast or check the concrete type.

**I — Interface Segregation**
Interfaces declare only what callers need. `IExerciseRepository` has `insertBatch`. When read operations are needed, add them as separate methods or a separate interface — never bloat an existing one.

**D — Dependency Inversion**
Use cases take `shared_ptr<IExerciseLoader>` and `shared_ptr<IExerciseRepository>` — never the concrete types. Concrete types appear only in `main.cpp` (the composition root).

---

## Naming conventions

| Thing | Convention | Example |
|---|---|---|
| Classes | PascalCase | `SeedExercisesUseCase` |
| Interfaces | `I` prefix | `IExerciseRepository` |
| Files | Match class name | `SeedExercisesUseCase.hpp` |
| Namespaces | lowercase | `domain`, `application`, `infrastructure`, `presentation` |
| Methods | camelCase | `insertBatch`, `metValue` |
| Private members | camelCase + `_` suffix | `loader_`, `dbClient_` |
| Enums | PascalCase values | `ExerciseCategory::Cardio` |

---

## Async rules (Drogon event loop)

- **Never block the event loop thread.** No `sleep`, no `future::get()`, no synchronous network calls.
- All I/O (DB, HTTP) uses the **`onSuccess` / `onError` callback pair**.
- **Exactly one** callback fires per operation, exactly once.
- Any object captured in an async lambda **must be owned by `shared_ptr`**. Never capture raw pointers or references — the call stack that created them will be gone when the callback fires.
- Infrastructure adapters that capture `this` in a lambda **must inherit `enable_shared_from_this`** and capture `shared_from_this()`.
- File I/O (local disk, small data) may be synchronous inside handlers; it is fast enough that it does not block the loop materially.

## Lifetime pattern for new handlers

```cpp
// In main.cpp, inside a registerHandler lambda:
auto repo    = std::make_shared<ConcreteRepository>(drogon::app().getDbClient());
auto useCase = std::make_shared<SomeUseCase>(dependency, repo);
auto ctrl    = std::make_shared<SomeController>(useCase);
ctrl->handle(req, std::move(cb));
// repo, useCase, ctrl stay alive because useCase/ctrl hold shared_ptrs,
// and the async callbacks capture repo via shared_ptr copies.
```

---

## Adding a new feature — checklist

1. **Entity**: `src/domain/entities/NewEntity.hpp` — plain struct, no external includes.
2. **Repository interface**: `src/domain/repositories/INewEntityRepository.hpp`.
3. **Use case**: `src/application/use_cases/DoSomethingUseCase.hpp/.cpp`.
4. **Adapter (DB)**: `src/infrastructure/persistence/PostgresNewEntityRepository.hpp/.cpp` — implements the interface.
5. **Controller**: `src/presentation/controllers/SomeController.hpp/.cpp` — HTTP → use case.
6. **Wire in `main.cpp`**: create `shared_ptr`s, register handler.
7. **SQL**: add migration or update `sql/init.sql`.

---

## Error handling

- Infrastructure errors propagate via `onError(std::string message)`.
- Presentation maps errors to HTTP codes: `500` for infra errors, `400` for invalid input, `409` for conflicts.
- Never throw from inside an async callback — uncaught exceptions in Drogon's event loop crash the process.
- Return early when an error is detected; never call both callbacks.

---

## What NOT to do

- Do not add Drogon includes to `src/domain/` or `src/application/`.
- Do not put SQL strings in use cases or controllers.
- Do not put business logic (calorie formulas, MET calculations) in repositories or controllers.
- Do not create global singletons; use `shared_ptr` + constructor injection.
- Do not use `new` / raw pointers for heap objects — use `make_shared`.
