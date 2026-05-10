#pragma once
#include "application/ports/IPasswordHasher.hpp"

namespace infrastructure {

class PasswordHasher : public application::IPasswordHasher {
public:
    std::string hash(const std::string& password) override;
    bool verify(const std::string& password, const std::string& storedHash) override;
};

} // namespace infrastructure
