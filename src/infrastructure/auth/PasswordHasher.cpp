#include "infrastructure/auth/PasswordHasher.hpp"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace infrastructure {

static std::string toHex(const unsigned char* data, size_t len)
{
    std::ostringstream oss;
    for (size_t i = 0; i < len; ++i)
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]);
    return oss.str();
}

static std::vector<unsigned char> fromHex(const std::string& hex)
{
    std::vector<unsigned char> bytes;
    bytes.reserve(hex.size() / 2);
    for (size_t i = 0; i + 1 < hex.size(); i += 2) {
        unsigned char b = static_cast<unsigned char>(std::stoul(hex.substr(i, 2), nullptr, 16));
        bytes.push_back(b);
    }
    return bytes;
}

// Stored format: "<iterations>:<salt_hex>:<hash_hex>"
std::string PasswordHasher::hash(const std::string& password)
{
    constexpr int kIterations = 100000;
    constexpr int kSaltLen    = 32;
    constexpr int kHashLen    = 32;

    unsigned char salt[kSaltLen];
    if (RAND_bytes(salt, kSaltLen) != 1)
        throw std::runtime_error("RAND_bytes failed");

    unsigned char derived[kHashLen];
    if (PKCS5_PBKDF2_HMAC(
            password.c_str(), static_cast<int>(password.size()),
            salt, kSaltLen,
            kIterations,
            EVP_sha256(),
            kHashLen, derived) != 1)
        throw std::runtime_error("PBKDF2 failed");

    return std::to_string(kIterations) + ":"
         + toHex(salt, kSaltLen) + ":"
         + toHex(derived, kHashLen);
}

bool PasswordHasher::verify(const std::string& password, const std::string& storedHash)
{
    // Parse "<iterations>:<salt_hex>:<hash_hex>"
    auto pos1 = storedHash.find(':');
    if (pos1 == std::string::npos) return false;
    auto pos2 = storedHash.find(':', pos1 + 1);
    if (pos2 == std::string::npos) return false;

    int iterations = std::stoi(storedHash.substr(0, pos1));
    auto salt      = fromHex(storedHash.substr(pos1 + 1, pos2 - pos1 - 1));
    auto expected  = fromHex(storedHash.substr(pos2 + 1));

    constexpr int kHashLen = 32;
    unsigned char derived[kHashLen];
    if (PKCS5_PBKDF2_HMAC(
            password.c_str(), static_cast<int>(password.size()),
            salt.data(), static_cast<int>(salt.size()),
            iterations,
            EVP_sha256(),
            kHashLen, derived) != 1)
        return false;

    if (static_cast<int>(expected.size()) != kHashLen) return false;

    // Constant-time comparison.
    unsigned char diff = 0;
    for (int i = 0; i < kHashLen; ++i)
        diff |= derived[i] ^ expected[static_cast<size_t>(i)];
    return diff == 0;
}

} // namespace infrastructure
