#include <gmpxx.h>
#include <openssl/bn.h>
#include <random>
#include <iostream>
#include <iomanip>
#include <string>
#include "../include/types.h"
#include "../include/util.h"

static const unsigned int real_precision = 64;

using namespace std;

bytevec hashPrime(const bytevec& in) {
  throw runtime_error("nyi");
}

int main(int argc, char* argv[]) {
  mt19937 engine;
  mpz_class mpz_seed;

  for (const int i : {1, 2}) {
    engine.seed(421);
    uniform_int_distribution<unsigned short> gen_byte(0, 255);

    bytevec seed(7);
    for (byte& si : seed) {
      si = static_cast<byte>(gen_byte(engine));
    }
    cout << "seed: ";
    for (const auto& i : seed)
      cout << (int)i << ' ';
    cout << endl;

    mpz_import(mpz_seed.get_mpz_t(), seed.size(), 1, sizeof(seed[0]), 1, 0, seed.data());
    string mpz_rep = mpz_seed.get_str(10);

    BIGNUM* bn = BN_secure_new();
    BN_bin2bn(seed.data(), seed.size(), bn);
    string bn_rep = print_bn(bn);
    BN_free(bn);

    cout << "bn:  " << bn_rep << endl;
    cout << "mpz: " << bn_rep << endl;
  }

  gmp_randclass primeGen(gmp_randinit_mt);
  primeGen.seed(mpz_seed);

  mpz_class prime;
  mpz_class sign;
  mpz_class maxint;
  mpz_pow_ui(maxint.get_mpz_t(), (2_mpz).get_mpz_t(), 50ul);
  mpz_cdiv_q(maxint.get_mpz_t(), maxint.get_mpz_t(), (6_mpz).get_mpz_t());

  int isprime = 0;
  int count = 0;
  const int max_count = 1000;
  while (isprime <= 0 && count < max_count) {
    prime = primeGen.get_z_range(maxint);
    sign = primeGen.get_z_bits(1);
    sign = 2*sign-1;
    prime = 6*prime+sign;
    isprime = mpz_probab_prime_p(prime.get_mpz_t(), 30);
    cout << count << ".  " << prime.get_str(10) << " is " << (isprime == 2 ? "certainly " : (isprime == 1 ? "maybe " : "not ")) << "a prime." << endl;
    count++;
  }

  return 0;
}

