#ifndef _HASH_H_
#define _HASH_H_

#include "../include/Hash.h"
#include <exception>
#include <openssl/rand.h>

Hash::Hash(const int _lambda, const unsigned int _key_size, const unsigned int _block_size) :
  lambda(_lambda), ctx(EVP_CIPHER_CTX_free_ptr(EVP_CIPHER_CTX_new(), ::EVP_CIPHER_CTX_free)) {
  const unsigned int key_size = _key_size;
  const unsigned int block_size = _block_size;
  if (block_size != 128/divisor) {
    throw std::runtime_error("block_size not yet implemented");
  }
  switch (key_size*divisor) {
    case 128:
      cipher = EVP_aes_128_ctr;
      break;
    case 192:
      cipher = EVP_aes_192_ctr;
      break;
    case 256:
      cipher = EVP_aes_256_ctr;
      break;
    default:
      throw std::runtime_error("key_size not yet implemented");
  }

  int error = 0;
  key.resize(key_size);
  error = RAND_bytes(key.data(), key_size);
  if (error != 1)
    throw std::runtime_error("Failed to create 'key'");
  iv.resize(block_size);
  error = RAND_bytes(iv.data(), block_size);
  if (error != 1)
    throw std::runtime_error("Failed to create 'iv'");

  EVP_add_cipher(cipher());

  error = EVP_EncryptInit_ex(ctx.get(), cipher(), NULL, key.data(), iv.data());
  if (error != 1)
    throw std::runtime_error("EVP_EncryptInit_ex failed");
}

Hash::Hash(const int _lambda, const bytevec& _key, const bytevec& _iv) :
  lambda(_lambda), ctx(EVP_CIPHER_CTX_free_ptr(EVP_CIPHER_CTX_new(), ::EVP_CIPHER_CTX_free)) {
  key = _key;  // copy
  iv = _iv;  // copy
  const unsigned int key_size = key.size();
  const unsigned int block_size = iv.size();

  if (block_size != 128/divisor) {
    throw std::runtime_error("block_size not yet implemented");
  }
  switch (key_size*divisor) {
    case 128:
      cipher = EVP_aes_128_ctr;
      break;
    case 192:
      cipher = EVP_aes_192_ctr;
      break;
    case 256:
      cipher = EVP_aes_256_ctr;
      break;
    default:
      throw std::runtime_error("key_size not yet implemented");
  }

  EVP_add_cipher(cipher());

  int error = 0;
  error = EVP_EncryptInit_ex(ctx.get(), cipher(), NULL, key.data(), iv.data());
  if (error != 1)
    throw std::runtime_error("EVP_EncryptInit_ex failed");
}

const void Hash::hash(const BIGNUM* in, BIGNUM* out) const {
  // put data into memory-safe containers (idea from the openssl webpage)
  ptext.resize(BN_num_bytes(in));
  BN_bn2bin(in, &ptext[0]);

  // the output also goes first into a safe container
  ctext.resize(ptext.size()+get_block_size());
  int out_len1 = (int)ctext.size();

  // encryption
  int error = 0;
  error = EVP_EncryptUpdate(ctx.get(), &ctext[0], &out_len1, (const byte*)&ptext[0], (int)ptext.size());
  if (error != 1)
    throw std::runtime_error("EVP_EncryptUpdate failed");
  if (true) {
    ctext.resize(out_len1);
  } else {  // we don't need this I think. in case we do, EVP_EncryptInit_ex should also be called here in hash()
    int out_len2 = (int)ctext.size() - out_len1;
    error = EVP_EncryptFinal_ex(ctx.get(), &ctext[0]+out_len1, &out_len2);
    if (error != 1)
      throw std::runtime_error("EVP_EncryptFinal_ex failed");
    ctext.resize(out_len1 + out_len2);
  }

  // convert to BIGNUM
  BN_bin2bn((const byte*)&ctext[0], (int)ctext.size(), out);
  BN_mask_bits(out, lambda);
}

const unsigned int Hash::get_key_size() const {
  return key.size();
}

const unsigned int Hash::get_block_size() const {
  return iv.size();
}

#endif  // HASH_H_

