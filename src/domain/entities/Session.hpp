#pragma once
#include <string>
#include <cstdint>

namespace domain {

struct Session {
    int64_t     id;
    int64_t     user_id;
    std::string token;      // 64 hex chars
    std::string expires_at; // ISO 8601
    std::string created_at;
};

} // namespace domain
