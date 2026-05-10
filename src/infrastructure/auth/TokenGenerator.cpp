#include "infrastructure/auth/TokenGenerator.hpp"
#include <openssl/rand.h>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace infrastructure {

std::string TokenGenerator::generate()
{
    constexpr int kBytes = 32;
    unsigned char buf[kBytes];
    if (RAND_bytes(buf, kBytes) != 1)
        throw std::runtime_error("RAND_bytes failed");

    std::ostringstream oss;
    for (int i = 0; i < kBytes; ++i)
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(buf[i]);
    return oss.str(); // 64 lowercase hex characters
}

} // namespace infrastructure
