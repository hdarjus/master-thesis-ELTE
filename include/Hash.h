/*
 *
 * partly taken from
 * https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption
 *
 */

#include <openssl/evp.h>
#include <openssl/bn.h>
#include "types.h"
#include <functional>

class Hash {
  Hash(const unsigned int key_size, const unsigned int block_size, const int lambda);
  ~Hash();

  const void hash(BIGNUM* in, BIGNUM** out) const;

private:
  byte* key;
  byte* iv;
  const int lambda;
  std::function<EVP_CIPHER*()> cipher;
};

