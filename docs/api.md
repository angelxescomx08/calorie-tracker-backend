# Calorie Tracker API

Base URL: `http://localhost:8080`

All protected endpoints require the header:
```
Authorization: Bearer <token>
```

Tokens are 64-character hex strings that expire **14 days** after issue.

---

## Auth

### POST /auth/register
Register a new user.

**Body**
```json
{ "name": "John Doe", "email": "john@example.com", "password": "secret123" }
```
*`password` must be ≥ 8 characters.*

**Response 201**
```json
{
  "user": { "id": 1, "email": "john@example.com", "name": "John Doe",
            "profile_picture": null, "email_verified": false,
            "created_at": "...", "updated_at": "..." },
  "token": "64hexchars...",
  "expires_at": "2024-03-01T12:00:00Z"
}
```

---

### POST /auth/login
Authenticate an existing user.

**Body**
```json
{ "email": "john@example.com", "password": "secret123" }
```

**Response 200** — same shape as register response.

---

### POST /auth/logout
🔒 Protected. Invalidates the current session.

**Response 204** No Content.

---

### GET /auth/me
🔒 Protected. Returns the authenticated user.

**Response 200**
```json
{ "id": 1, "email": "...", "name": "...", "profile_picture": null,
  "email_verified": false, "created_at": "...", "updated_at": "..." }
```

---

## Profile

### GET /profile
🔒 Protected. Get the authenticated user's profile.

**Response 200**
```json
{ "id": 1, "user_id": 1, "birth_date": "1990-01-15",
  "gender": "male", "height_cm": 175.0, "activity_level": "moderately_active",
  "created_at": "...", "updated_at": "..." }
```
**Response 404** if profile has not been set yet.

---

### PUT /profile
🔒 Protected. Create or update the user's profile.

**Body**
```json
{
  "birth_date": "1990-01-15",
  "gender": "male",
  "height_cm": 175.0,
  "activity_level": "moderately_active"
}
```
`gender`: `"male"` | `"female"`  
`activity_level`: `"sedentary"` | `"lightly_active"` | `"moderately_active"` | `"very_active"` | `"extra_active"`

**Response 200** — Profile object.

---

## Goals

### GET /goals/active
🔒 Protected. Returns the current active goal.

**Response 200**
```json
{ "id": 1, "user_id": 1, "goal_type": "lose_weight",
  "target_weight_kg": 75.0, "weekly_rate_kg": 0.5,
  "daily_calorie_target": 1800, "start_date": "2024-01-01",
  "end_date": null, "is_active": true,
  "created_at": "...", "updated_at": "..." }
```
**Response 404** if no active goal.

---

### POST /goals
🔒 Protected. Create a new goal (deactivates any existing active goal).

**Body**
```json
{
  "goal_type": "lose_weight",
  "target_weight_kg": 75.0,
  "weekly_rate_kg": 0.5,
  "daily_calorie_target": 1800,
  "start_date": "2024-01-01",
  "end_date": null
}
```
`goal_type`: `"lose_weight"` | `"gain_weight"` | `"maintain"`

**Response 201** — Goal object.

---

### PUT /goals/{id}
🔒 Protected. Update an existing goal.

**Body** — same fields as POST.  
**Response 200** — Updated goal object.

---

### DELETE /goals/{id}
🔒 Protected. Deactivate a goal.

**Response 204** No Content.

---

## Weight Logs

### GET /weight
🔒 Protected.

**Query params**
| Param | Description |
|-------|-------------|
| `start_date` | YYYY-MM-DD — filter from date (optional) |
| `end_date`   | YYYY-MM-DD — filter to date (optional) |
| `limit`      | Max records, default 100 |

**Response 200**
```json
{ "items": [ { "id": 1, "user_id": 1, "weight_kg": 80.5,
               "logged_date": "2024-01-10", "notes": null, "created_at": "..." } ] }
```

---

### POST /weight
🔒 Protected.

**Body**
```json
{ "weight_kg": 80.5, "logged_date": "2024-01-10", "notes": "Morning weight" }
```
**Response 201** — Weight log object.

---

### PUT /weight/{id}
🔒 Protected. Update a weight log entry.

**Body** — same as POST.  
**Response 200** — Updated weight log object.

---

### DELETE /weight/{id}
🔒 Protected.  
**Response 204** No Content.

---

## Body Measurements

### GET /measurements
🔒 Protected.

**Query params**: `start_date`, `end_date` (optional, YYYY-MM-DD)

**Response 200**
```json
{ "items": [ {
  "id": 1, "user_id": 1, "measured_date": "2024-01-10",
  "neck_cm": 38.0, "waist_cm": 85.0, "hip_cm": null,
  "body_fat_percentage": 18.5, "notes": null, "created_at": "..."
} ] }
```

---

### POST /measurements
🔒 Protected. Body fat % is computed automatically using the US Navy formula if height is available in profile.

**Body**
```json
{
  "measured_date": "2024-01-10",
  "neck_cm": 38.0,
  "waist_cm": 85.0,
  "hip_cm": null,
  "notes": ""
}
```
`hip_cm` is required for women (used in female US Navy formula).

**Response 201** — Measurement object.

---

### DELETE /measurements/{id}
🔒 Protected.  
**Response 204** No Content.

---

## Foods

### GET /foods
🔒 Protected. Search the food database.

**Query params**
| Param    | Description |
|----------|-------------|
| `q`      | Search query (name, brand) |
| `limit`  | Default 20 |
| `offset` | Default 0 |

**Response 200**
```json
{
  "items": [ { "id": 1, "name": "Banana", "brand": null, "barcode": null,
               "serving_size_g": 100.0, "calories": 89.0,
               "protein_g": 1.1, "carbs_g": 23.0, "fat_g": 0.3,
               "fiber_g": 2.6, "sugar_g": 12.0, "sodium_mg": 1.0,
               "created_by_user_id": null, "is_verified": true,
               "created_at": "...", "updated_at": "..." } ],
  "total": 1, "limit": 20, "offset": 0
}
```

---

### GET /foods/{id}
🔒 Protected.  
**Response 200** — Food object. **Response 404** if not found.

---

### POST /foods
🔒 Protected. Create a custom food.

**Body**
```json
{
  "name": "My Protein Bar",
  "brand": "My Brand",
  "barcode": "123456789",
  "serving_size_g": 60.0,
  "calories": 250.0,
  "protein_g": 20.0,
  "carbs_g": 25.0,
  "fat_g": 8.0,
  "fiber_g": 3.0,
  "sugar_g": 5.0,
  "sodium_mg": 150.0
}
```
**Response 201** — Food object.

---

### PUT /foods/{id}
🔒 Protected.  
**Body** — same as POST (minus `barcode`).  
**Response 200** — Updated food object.

---

### DELETE /foods/{id}
🔒 Protected. Only the creator can delete their food.  
**Response 204** No Content.

---

## Daily Log

### GET /daily
🔒 Protected. Returns the daily log with all meal entries. Creates an empty log if none exists.

**Query params**: `date` (YYYY-MM-DD, defaults to today)

**Response 200**
```json
{
  "id": 1, "user_id": 1, "log_date": "2024-01-10",
  "water_ml": 1500, "notes": null,
  "created_at": "...", "updated_at": "...",
  "meal_entries": [
    { "id": 1, "daily_log_id": 1, "meal_type": "breakfast",
      "food_id": 5, "quantity_g": 150.0,
      "calories": 133.5, "protein_g": 1.65, "carbs_g": 34.5, "fat_g": 0.45,
      "created_at": "..." }
  ]
}
```

---

### PUT /daily/{id}
🔒 Protected. Update water intake and notes for a daily log.

**Body**
```json
{ "water_ml": 2000, "notes": "Felt great today" }
```
**Response 200** — Daily log object (without meal entries).

---

### POST /daily/meals
🔒 Protected. Add a meal entry. Macros are computed as a proportion of the food's serving size.

**Body**
```json
{
  "date": "2024-01-10",
  "meal_type": "breakfast",
  "food_id": 5,
  "quantity_g": 150.0
}
```
`meal_type`: `"breakfast"` | `"lunch"` | `"dinner"` | `"snack"`

**Response 201** — Meal entry object.

---

### PUT /daily/meals/{entry_id}/{log_id}
🔒 Protected. Update a meal entry quantity or type.

**Body**
```json
{ "meal_type": "lunch", "food_id": 5, "quantity_g": 200.0 }
```
**Response 200** — Updated meal entry object.

---

### DELETE /daily/meals/{entry_id}/{log_id}
🔒 Protected.  
**Response 204** No Content.

---

## Exercises

### GET /exercises
🔒 Protected. Search the exercise database (1,111 pre-loaded from PA Compendium 2024).

**Query params**
| Param      | Description |
|------------|-------------|
| `q`        | Name/description search |
| `category` | Filter: `cardio` | `strength` | `flexibility` | `sports` | `other` |
| `limit`    | Default 20 |
| `offset`   | Default 0 |

**Response 200**
```json
{
  "items": [ { "id": 1, "name": "Running, 8 km/h", "category": "cardio",
               "met_value": 8.3, "description": "...",
               "created_by_user_id": null, "is_verified": true, "created_at": "..." } ],
  "total": 1111, "limit": 20, "offset": 0
}
```

---

### GET /exercises/{id}
🔒 Protected.  
**Response 200** — Exercise object. **Response 404** if not found.

---

### POST /exercises
🔒 Protected. Create a custom exercise.

**Body**
```json
{
  "name": "Paddle Tennis",
  "category": "sports",
  "met_value": 6.0,
  "description": "Moderate intensity paddle tennis"
}
```
**Response 201** — Exercise object.

---

## Exercise Logs

### GET /exercise-logs
🔒 Protected. List exercise logs for a given date.

**Query params**: `date` (YYYY-MM-DD, defaults to today)

**Response 200**
```json
{ "items": [ {
  "id": 1, "user_id": 1, "exercise_id": 42,
  "log_date": "2024-01-10", "duration_minutes": 30,
  "calories_burned": 249.0,
  "sets": null, "reps": null, "distance_km": 4.0, "avg_heart_rate": 145,
  "notes": "Morning run", "created_at": "..."
} ] }
```

---

### POST /exercise-logs
🔒 Protected. Log an exercise session. `calories_burned` is computed automatically as `MET × body_weight_kg × (duration_min / 60)` using the user's latest weight log (falls back to 70 kg if none available).

**Body**
```json
{
  "exercise_id": 42,
  "log_date": "2024-01-10",
  "duration_minutes": 30,
  "sets": null,
  "reps": null,
  "distance_km": 4.0,
  "avg_heart_rate": 145,
  "notes": "Morning run"
}
```
**Response 201** — Exercise log object.

---

### PUT /exercise-logs/{id}
🔒 Protected.

**Body** — same fields as POST plus `calories_burned` (if you want to override the calculation).  
**Response 200** — Updated exercise log object.

---

### DELETE /exercise-logs/{id}
🔒 Protected.  
**Response 204** No Content.

---

## Error Responses

All errors follow:
```json
{ "error": "Human-readable error message" }
```

| Status | Meaning |
|--------|---------|
| 400    | Bad request / validation error |
| 401    | Missing or invalid / expired token |
| 403    | Forbidden (ownership check failed) |
| 404    | Resource not found |
| 500    | Internal server error |
