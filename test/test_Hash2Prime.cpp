#ifndef _DEBUG
#define _DEBUG
#endif

#include "../include/Hash2Prime.h"
#include "../include/util.h"
#include <openssl/bn.h>
#include <openssl/rand.h>
#include <iostream>

int main(int argc, char* argv[]) {
  BN_CTX* ctx = BN_CTX_secure_new();
  BN_CTX_start(ctx);

  BIGNUM* p = BN_CTX_get(ctx);
  BIGNUM* q = BN_CTX_get(ctx);
  Hash2Prime hash(20);  // around 1 million primes
  // the 1 millionth prime is 15,485,863
  // see: https://primes.utm.edu/lists/small/millions/
  const int nbits = 256;
  bytevec cache(nbits/CHAR_BIT);
  for (int i = 0; i < 10; i++) {
    int rc = RAND_bytes(cache.data(), cache.size());
    if (rc != 1)
      throw std::runtime_error("RAND_bytes cache failed");
    BN_bin2bn(cache.data(), cache.size(), p);
    std::cout << "Hashing " << print_bn(p) << " ..." << std::endl;

    hash(p, q);

    int isprime = BN_is_prime_ex(q, BN_prime_checks, ctx, NULL);
    std::cout << print_bn(q) << " is " << (isprime == 0 ? "not" : (isprime == 1 ? "maybe" : "certainly")) << " a prime." << std::endl;
  }

  BN_CTX_end(ctx);
  BN_CTX_free(ctx);
}
