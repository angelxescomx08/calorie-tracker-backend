#pragma once
#include <string>
#include <cstdint>

namespace domain {

struct User {
    int64_t     id;
    std::string email;
    std::string name;
    std::string profile_picture; // empty = not set
    bool        email_verified;
    std::string created_at;
    std::string updated_at;
};

} // namespace domain
