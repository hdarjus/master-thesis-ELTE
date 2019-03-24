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
  Hash(const int _lambda, const unsigned int _key_size = 256/divisor, const unsigned int _block_size = 128/divisor);
  Hash(const int _lambda, const bytevec& _key, const bytevec& _iv);
  ~Hash() = default;

  const void hash(const BIGNUM* in, BIGNUM* out) const;

  const unsigned int get_key_size() const;
  const unsigned int get_block_size() const;

  static const unsigned int divisor = CHAR_BIT*sizeof(byte);

private:
  const int lambda;

  // cipher function
  std::function<const EVP_CIPHER*()> cipher;

  // unique_ptr members for memory safety
  bytevec key;
  bytevec iv;
  EVP_CIPHER_CTX_free_ptr ctx;

  // helpers for memory safety
  mutable secure_string ctext;
  mutable secure_string ptext;
};

