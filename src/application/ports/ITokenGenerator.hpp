#pragma once
#include <string>

namespace application {

class ITokenGenerator {
public:
    virtual ~ITokenGenerator() = default;
    virtual std::string generate() = 0; // Returns 64 lowercase hex characters.
};

} // namespace application
