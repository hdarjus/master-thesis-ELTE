#include <openssl/evp.h>
#include <memory>
#include "zallocator.hpp"

using byte = unsigned char;
using byte_ptr = std::unique_ptr<byte>;
using secure_string = std::basic_string<byte, std::char_traits<byte>, zallocator<byte> >;
using EVP_CIPHER_CTX_free_ptr = std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)>;

