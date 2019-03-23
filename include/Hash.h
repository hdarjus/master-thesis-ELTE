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

class Hash {  // probably implement this as an std::hash struct?
public:
  Hash(const int lambda, const unsigned int key_size = 256/(CHAR_BIT*sizeof(byte)), const unsigned int block_size = 128/(CHAR_BIT*sizeof(byte)));
  ~Hash() = default;

  const void hash(const BIGNUM* in, BIGNUM* out) const;

private:
  unsigned int key_size;
  unsigned int block_size;
  const int lambda;

  // cipher function
  std::function<const EVP_CIPHER*()> cipher;

  // unique_ptr members for memory safety
  byte_ptr key;
  byte_ptr iv;
  EVP_CIPHER_CTX_free_ptr ctx;

  // helpers for memory safety
  mutable secure_string ctext;
  mutable secure_string ptext;
};

