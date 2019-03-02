#ifndef _SAFE_PRIMES_H_
#define _SAFE_PRIMES_H_

extern "C" {
#include <openssl/bn.h>
}

#include <string>

class SafePrime {
public:
  SafePrime();
  ~SafePrime();

  void generate (const int size = 32);
  const std::string print () const;

  static void seed (const std::string buffer);

private:
  BIGNUM* value;
  std::string str;
};

#endif  // _SAFE_PRIMES_H_
