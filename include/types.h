#include <openssl/evp.h>
#include <memory>
#include <vector>
#include <utility>
#include "zallocator.hpp"

using byte = unsigned char;
using byte_ptr = std::unique_ptr<byte>;
using bytevec = std::vector<byte>;
using secure_string = std::basic_string<byte, std::char_traits<byte>, zallocator<byte> >;
using EVP_CIPHER_CTX_free_ptr = std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)>;
using money = int;
using proof_wesolowski = bytevec;
using proof_pietrzak = std::vector<bytevec>;
enum class VDF_version {WESOLOWSKI, PIETRZAK};

template<typename proof>
using solution = std::pair<proof, bytevec>;

