-- ============================================================
-- Calorie Tracker Backend — Init Schema
-- PostgreSQL 17
-- ============================================================

-- ─────────────────────────────────────────────
-- ENUMS
-- ─────────────────────────────────────────────

CREATE TYPE auth_provider_type AS ENUM ('google', 'email');

CREATE TYPE gender_type AS ENUM ('male', 'female');

CREATE TYPE activity_level_type AS ENUM (
  'sedentary',
  'lightly_active',
  'moderately_active',
  'very_active',
  'extra_active'
);

CREATE TYPE goal_type AS ENUM ('lose_weight', 'gain_weight', 'maintain');

CREATE TYPE meal_type AS ENUM ('breakfast', 'lunch', 'dinner', 'snack');

CREATE TYPE exercise_category AS ENUM ('cardio', 'strength', 'flexibility', 'sports', 'other');

-- ─────────────────────────────────────────────
-- GRUPO: AUTENTICACIÓN
-- ─────────────────────────────────────────────

CREATE TABLE users (
  id              BIGSERIAL       PRIMARY KEY,
  email           VARCHAR(255)    NOT NULL UNIQUE,
  name            VARCHAR(255)    NOT NULL,
  profile_picture VARCHAR(500),
  email_verified  BOOLEAN         NOT NULL DEFAULT FALSE,
  created_at      TIMESTAMPTZ     NOT NULL DEFAULT NOW(),
  updated_at      TIMESTAMPTZ     NOT NULL DEFAULT NOW()
);

CREATE TABLE auth_providers (
  id                       BIGSERIAL           PRIMARY KEY,
  user_id                  BIGINT              NOT NULL REFERENCES users(id) ON DELETE CASCADE,
  provider                 auth_provider_type  NOT NULL,

  -- Email/contraseña
  password_hash            VARCHAR(255),
  email_verification_token VARCHAR(100),
  token_expires_at         TIMESTAMPTZ,

  -- Google OAuth
  google_sub               VARCHAR(255),
  google_access_token      TEXT,
  google_refresh_token     TEXT,
  google_token_expires_at  TIMESTAMPTZ,

  created_at               TIMESTAMPTZ         NOT NULL DEFAULT NOW(),
  updated_at               TIMESTAMPTZ         NOT NULL DEFAULT NOW(),

  CONSTRAINT uq_auth_user_provider UNIQUE (user_id, provider)
);

CREATE INDEX idx_auth_google_sub ON auth_providers(google_sub);

-- ─────────────────────────────────────────────
-- GRUPO: PERFIL Y OBJETIVOS
-- ─────────────────────────────────────────────

CREATE TABLE user_profiles (
  id             BIGSERIAL            PRIMARY KEY,
  user_id        BIGINT               NOT NULL UNIQUE REFERENCES users(id) ON DELETE CASCADE,
  birth_date     DATE                 NOT NULL,
  gender         gender_type          NOT NULL,
  height_cm      DECIMAL(5,2)         NOT NULL,
  activity_level activity_level_type  NOT NULL,
  created_at     TIMESTAMPTZ          NOT NULL DEFAULT NOW(),
  updated_at     TIMESTAMPTZ          NOT NULL DEFAULT NOW()
);

CREATE TABLE user_goals (
  id                   BIGSERIAL     PRIMARY KEY,
  user_id              BIGINT        NOT NULL REFERENCES users(id) ON DELETE CASCADE,
  goal_type            goal_type     NOT NULL,
  target_weight_kg     DECIMAL(5,2)  NOT NULL,
  weekly_rate_kg       DECIMAL(3,2)  NOT NULL DEFAULT 0.5,
  daily_calorie_target INT           NOT NULL,
  start_date           DATE          NOT NULL,
  end_date             DATE,
  is_active            BOOLEAN       NOT NULL DEFAULT TRUE,
  created_at           TIMESTAMPTZ   NOT NULL DEFAULT NOW(),
  updated_at           TIMESTAMPTZ   NOT NULL DEFAULT NOW()
);

CREATE INDEX idx_goals_user_active ON user_goals(user_id, is_active);

-- ─────────────────────────────────────────────
-- GRUPO: SEGUIMIENTO CORPORAL
-- ─────────────────────────────────────────────

CREATE TABLE weight_logs (
  id          BIGSERIAL     PRIMARY KEY,
  user_id     BIGINT        NOT NULL REFERENCES users(id) ON DELETE CASCADE,
  weight_kg   DECIMAL(5,2)  NOT NULL,
  logged_date DATE          NOT NULL,
  notes       VARCHAR(255),
  created_at  TIMESTAMPTZ   NOT NULL DEFAULT NOW()
);

CREATE INDEX idx_weight_user_date ON weight_logs(user_id, logged_date);

CREATE TABLE body_measurements (
  id                  BIGSERIAL     PRIMARY KEY,
  user_id             BIGINT        NOT NULL REFERENCES users(id) ON DELETE CASCADE,
  measured_date       DATE          NOT NULL,
  neck_cm             DECIMAL(5,2)  NOT NULL,
  waist_cm            DECIMAL(5,2)  NOT NULL,
  hip_cm              DECIMAL(5,2),
  body_fat_percentage DECIMAL(5,2),
  notes               VARCHAR(255),
  created_at          TIMESTAMPTZ   NOT NULL DEFAULT NOW()
);

CREATE INDEX idx_measurements_user_date ON body_measurements(user_id, measured_date);

-- ─────────────────────────────────────────────
-- GRUPO: NUTRICIÓN
-- ─────────────────────────────────────────────

CREATE TABLE foods (
  id                 BIGSERIAL     PRIMARY KEY,
  name               VARCHAR(255)  NOT NULL,
  brand              VARCHAR(255),
  barcode            VARCHAR(50),
  serving_size_g     DECIMAL(7,2)  NOT NULL DEFAULT 100,
  calories           DECIMAL(7,2)  NOT NULL,
  protein_g          DECIMAL(7,2)  NOT NULL DEFAULT 0,
  carbs_g            DECIMAL(7,2)  NOT NULL DEFAULT 0,
  fat_g              DECIMAL(7,2)  NOT NULL DEFAULT 0,
  fiber_g            DECIMAL(7,2)  DEFAULT 0,
  sugar_g            DECIMAL(7,2)  DEFAULT 0,
  sodium_mg          DECIMAL(7,2)  DEFAULT 0,
  created_by_user_id BIGINT        REFERENCES users(id) ON DELETE SET NULL,
  is_verified        BOOLEAN       NOT NULL DEFAULT FALSE,
  created_at         TIMESTAMPTZ   NOT NULL DEFAULT NOW(),
  updated_at         TIMESTAMPTZ   NOT NULL DEFAULT NOW()
);

CREATE INDEX idx_foods_name    ON foods(name);
CREATE INDEX idx_foods_barcode ON foods(barcode);

CREATE TABLE daily_logs (
  id         BIGSERIAL     PRIMARY KEY,
  user_id    BIGINT        NOT NULL REFERENCES users(id) ON DELETE CASCADE,
  log_date   DATE          NOT NULL,
  water_ml   INT           NOT NULL DEFAULT 0,
  notes      VARCHAR(500),
  created_at TIMESTAMPTZ   NOT NULL DEFAULT NOW(),
  updated_at TIMESTAMPTZ   NOT NULL DEFAULT NOW(),

  CONSTRAINT uq_daily_log_user_date UNIQUE (user_id, log_date)
);

CREATE TABLE meal_entries (
  id           BIGSERIAL     PRIMARY KEY,
  daily_log_id BIGINT        NOT NULL REFERENCES daily_logs(id) ON DELETE CASCADE,
  meal_type    meal_type     NOT NULL,
  food_id      BIGINT        NOT NULL REFERENCES foods(id),
  quantity_g   DECIMAL(7,2)  NOT NULL,
  calories     DECIMAL(7,2)  NOT NULL,
  protein_g    DECIMAL(7,2)  NOT NULL,
  carbs_g      DECIMAL(7,2)  NOT NULL,
  fat_g        DECIMAL(7,2)  NOT NULL,
  created_at   TIMESTAMPTZ   NOT NULL DEFAULT NOW()
);

-- ─────────────────────────────────────────────
-- GRUPO: EJERCICIO
-- ─────────────────────────────────────────────

CREATE TABLE exercises (
  id                 BIGSERIAL          PRIMARY KEY,
  name               VARCHAR(255)       NOT NULL,
  category           exercise_category  NOT NULL,
  met_value          DECIMAL(5,2)       NOT NULL,
  description        TEXT,
  created_by_user_id BIGINT             REFERENCES users(id) ON DELETE SET NULL,
  is_verified        BOOLEAN            NOT NULL DEFAULT FALSE,
  created_at         TIMESTAMPTZ        NOT NULL DEFAULT NOW()
);

CREATE TABLE exercise_logs (
  id               BIGSERIAL     PRIMARY KEY,
  user_id          BIGINT        NOT NULL REFERENCES users(id) ON DELETE CASCADE,
  exercise_id      BIGINT        NOT NULL REFERENCES exercises(id),
  log_date         DATE          NOT NULL,
  duration_minutes INT           NOT NULL,
  calories_burned  DECIMAL(7,2)  NOT NULL,
  sets             SMALLINT,
  reps             SMALLINT,
  distance_km      DECIMAL(7,3),
  avg_heart_rate   SMALLINT,
  notes            VARCHAR(255),
  created_at       TIMESTAMPTZ   NOT NULL DEFAULT NOW()
);

CREATE INDEX idx_exercise_log_user_date ON exercise_logs(user_id, log_date);
