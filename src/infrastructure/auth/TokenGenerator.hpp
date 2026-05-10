#pragma once
#include "application/ports/ITokenGenerator.hpp"

namespace infrastructure {

class TokenGenerator : public application::ITokenGenerator {
public:
    std::string generate() override;
};

} // namespace infrastructure
