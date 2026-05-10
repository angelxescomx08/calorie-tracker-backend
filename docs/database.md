# Base de Datos — Calorie Tracker

> Visualizar en dbdiagram.io: pegar el contenido de `database.dbml` en [dbdiagram.io](https://dbdiagram.io)

## Resumen

| Grupo         | Tablas                                          | Propósito                            |
|---------------|-------------------------------------------------|--------------------------------------|
| Autenticación | `users`, `auth_providers`                       | Login con Google y email/contraseña  |
| Perfil        | `user_profiles`, `user_goals`                   | Datos físicos y objetivos            |
| Cuerpo        | `weight_logs`, `body_measurements`              | Peso diario y % grasa corporal       |
| Nutrición     | `foods`, `daily_logs`, `meal_entries`           | Alimentos y comidas del día          |
| Ejercicio     | `exercises`, `exercise_logs`                    | Catálogo y registro de actividad     |

---

## Autenticación

### `users`
Tabla central. Almacena el perfil público del usuario.

- Un usuario puede tener **una o dos filas** en `auth_providers` (Google y/o email).
- `email_verified` refleja el estado del método email; siempre `true` si solo usa Google.

### `auth_providers`
Admite múltiples proveedores por usuario. Restricción única: `(user_id, provider)`.

| Provider | Campos usados                                                            |
|----------|--------------------------------------------------------------------------|
| `email`  | `password_hash` (bcrypt), `email_verification_token`, `token_expires_at` |
| `google` | `google_sub` (JWT sub), `google_access_token`, `google_refresh_token`    |

---

## Perfil y Objetivos

### `user_profiles`
Contiene los datos físicos invariables del usuario. Relación **1:1** con `users`.

Necesarios para calcular **BMR** (Mifflin-St Jeor):

```
Hombre: BMR = 10×peso_kg + 6.25×altura_cm − 5×edad + 5
Mujer:  BMR = 10×peso_kg + 6.25×altura_cm − 5×edad − 161
```

La edad se calcula en tiempo de ejecución desde `birth_date`.

**TDEE** (gasto energético total diario):

| Nivel de actividad   | Multiplicador |
|----------------------|---------------|
| `sedentary`          | × 1.200       |
| `lightly_active`     | × 1.375       |
| `moderately_active`  | × 1.550       |
| `very_active`        | × 1.725       |
| `extra_active`       | × 1.900       |

### `user_goals`
Define el objetivo actual del usuario. Solo debe haber **una fila activa** (`is_active = true`) por usuario.

Fórmula para calcular `daily_calorie_target` al crear un objetivo:

```
1 kg de grasa ≈ 7700 kcal
0.5 kg/semana → déficit/superávit de 550 kcal/día
→ factor = weekly_rate_kg × 1100

Perder peso:   daily_calorie_target = TDEE − factor
Ganar peso:    daily_calorie_target = TDEE + factor
Mantener:      daily_calorie_target = TDEE
```

---

## Seguimiento Corporal

### `weight_logs`
Registro diario del peso. Se recomienda registrar **1 vez por día**, en la mañana en ayunas.

El peso más reciente hasta una fecha se usa también para calcular calorías quemadas por ejercicio.

### `body_measurements`
Permite calcular el **% de grasa corporal** mediante el **Método US Navy**.

Medidas necesarias (todas en **centímetros**):

| Medida        | Hombre | Mujer |
|---------------|--------|-------|
| Cuello        | ✓      | ✓     |
| Cintura       | ✓      | ✓     |
| Cadera        | —      | ✓     |

**Fórmulas US Navy:**

```
Hombre:
  %GC = 86.010 × log10(cintura − cuello) − 70.041 × log10(altura) + 36.76

Mujer:
  %GC = 163.205 × log10(cintura + cadera − cuello) − 97.684 × log10(altura) − 78.387
```

> La altura se obtiene de `user_profiles.height_cm`.  
> El resultado se almacena en `body_fat_percentage` como snapshot.

**Cómo tomar las medidas:**
- **Cuello**: circunferencia justo debajo de la laringe.
- **Cintura**: circunferencia al nivel del ombligo (en hombres, al punto más ancho).
- **Cadera** (solo mujeres): circunferencia en el punto más ancho de las caderas/glúteos.

---

## Nutrición

### `foods`
Base de datos de alimentos. Puede ser **global** (`created_by_user_id = NULL`) o **creada por el usuario**.

Todos los valores nutricionales se expresan por `serving_size_g` (por defecto 100 g).

### `daily_logs`
Un registro por usuario por día. Es el **padre** de todas las entradas de comida del día.  
También registra el consumo de agua (`water_ml`).

### `meal_entries`
Cada fila representa un alimento consumido en una comida específica.

Los macros se calculan como **snapshot** en el momento del registro:
```
valor_macro = (foods.valor_macro / foods.serving_size_g) × quantity_g
```
Esto evita que ediciones futuras al alimento alteren el historial.

**Tipos de comida (`meal_type`):**
- `breakfast` — Desayuno
- `lunch` — Almuerzo
- `dinner` — Cena
- `snack` — Colación / Merienda

---

## Ejercicio

### `exercises`
Catálogo de ejercicios con su valor **MET** (Metabolic Equivalent of Task).

Valores MET de referencia (Compendium of Physical Activities, 2011):

| Ejercicio               | MET  | Ejercicio                | MET  |
|-------------------------|------|--------------------------|------|
| Caminar 3.5 km/h        | 2.8  | Ciclismo moderado        | 5.8  |
| Caminar 5 km/h          | 3.5  | Natación moderada        | 5.8  |
| Correr 8 km/h           | 8.0  | Fútbol                   | 7.0  |
| Correr 10 km/h          | 10.0 | Baloncesto               | 6.5  |
| Correr 12 km/h          | 11.5 | HIIT                     | 8.0  |
| Pesas (moderado)        | 3.5  | CrossFit                 | 8.0  |
| Pesas (intenso)         | 5.0  | Yoga                     | 2.5  |
| Escalera / Stair climb  | 4.0  | Pilates                  | 3.0  |
| Saltar la cuerda        | 10.0 | Elíptica moderada        | 5.0  |

### `exercise_logs`
Registra cada sesión de ejercicio del usuario.

**Fórmula de calorías quemadas:**
```
calories_burned = MET × peso_kg × (duration_minutes / 60.0)
```
El peso se obtiene del `weight_log` más reciente hasta `log_date`.

Campos opcionales por categoría:

| Campo           | Cardio | Fuerza |
|-----------------|--------|--------|
| `distance_km`   | ✓      | —      |
| `sets`          | —      | ✓      |
| `reps`          | —      | ✓      |
| `avg_heart_rate`| ✓      | ✓      |

---

## Diagrama de relaciones (resumen)

```
users
 ├── auth_providers (1:N — máx. 2)
 ├── user_profiles  (1:1)
 ├── user_goals     (1:N)
 ├── weight_logs    (1:N)
 ├── body_measurements (1:N)
 ├── daily_logs     (1:N)
 │    └── meal_entries (1:N)
 │         └── foods (N:1)
 └── exercise_logs  (1:N)
      └── exercises  (N:1)
```

---

## Decisiones de diseño

| Decisión | Razón |
|----------|-------|
| `auth_providers` separada de `users` | Permite agregar más proveedores OAuth en el futuro sin alterar la tabla principal |
| Macros como snapshot en `meal_entries` | Cambios en `foods` no deben alterar el historial nutricional del usuario |
| `body_fat_percentage` pre-calculado | Evita recalcular la fórmula logarítmica en cada consulta |
| `calories_burned` pre-calculado | El peso del usuario puede cambiar; el snapshot refleja el valor real del día |
| `daily_calorie_target` en `user_goals` | Evita recalcular TDEE en cada petición; se recalcula solo al cambiar el objetivo |
| `bigserial` como PK | Preparado para alto volumen; más eficiente que UUID para joins |
