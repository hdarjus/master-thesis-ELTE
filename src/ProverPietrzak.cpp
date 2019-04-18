#include "../include/ProverPietrzak.h"
#include <bitset>

ProverPietrzak::ProverPietrzak() :
    ctx_ptr(BN_CTX_free_ptr(BN_CTX_secure_new(), ::BN_CTX_free)) { }

typename ProverPietrzak::solution
ProverPietrzak::operator()(const VerifierPietrzak& verifier) const {
  BN_CTX* ctx = ctx_ptr.get();
  BN_CTX_start(ctx);

  // constants
  const auto puzzle = verifier.get_RSWPuzzle();
  const auto hash = verifier.get_Hash();
  const unsigned long lambda = puzzle.get_lambda();
  const unsigned long T = puzzle.get_T();
  const bytevec _N = puzzle.get_N();
  const bytevec _x = puzzle.get_x();

  // helper variables
  unsigned long Ti;  // denotes T_{i+1} from the paper
  unsigned long t = std::bitset<CHAR_BIT*sizeof(decltype(T))>(T-1).count();
  if (T != 1 << t) {
    throw std::runtime_error("T is not a two-power");
  }
  unsigned long d_max = t/2;
  BIGNUM* mu = BN_CTX_get(ctx);
  BIGNUM* mu_prime = BN_CTX_get(ctx);
  BIGNUM* N = BN_CTX_get(ctx);
  BIGNUM* x = BN_CTX_get(ctx);
  BIGNUM* r = BN_CTX_get(ctx);
  BIGNUM* y = BN_CTX_get(ctx);
  BIGNUM* xy_help = BN_CTX_get(ctx);
  BIGNUM* xymu = BN_CTX_get(ctx);
  BIGNUM* prod_help = BN_CTX_get(ctx);
  BIGNUM* prod_help2 = BN_CTX_get(ctx);
  std::vector<std::vector<BIGNUM*> > cache(d_max);
  std::vector<BIGNUM*> r_cache(d_max);
  for (int i = 1; i <= d_max; i++) {
    r_cache[i-1] = BN_CTX_get(ctx);
    cache[i-1].resize(exp2(i-1));
    for (int k = 0; k < cache[i-1].size(); k++) {
      cache[i-1][k] = BN_CTX_get(ctx);
    }
  }

  // set initial values
  Ti = T;
  BN_bin2bn(_x.data(), (int)_x.size(), x);
  BN_bin2bn(_N.data(), (int)_N.size(), N);
  BN_copy(y, x);

  // declare returned values
  bytevec _y;
  std::vector<bytevec> _mu_prime(t);

  // calculate _y and save partial results
  for (unsigned long d_current = 0; d_current < t; d_current++) {
    unsigned long two_to_d_current = 1 << d_current;
    unsigned long two_to_d_next = 1 << (d_current+1);
    unsigned long k = 0;
    for (unsigned long tt = two_to_d_current; tt < two_to_d_next; tt++) {
      BN_mod_sqr(y, y, N, ctx);
      if (t-d_current <= d_max &&
          tt == (2*k+1)*two_to_d_current) {
        BN_copy(cache[t-d_current-1][k], y);
        k++;
      }
    }
  }
  BN_mod_sqr(y, y, N, ctx);  // plus one squaring
  _y.resize(BN_num_bytes(y));
  BN_bn2bin(y, &_y[0]);

  // calculate xy_help that helps the hashing step
  BN_copy(xy_help, x);
  BN_lshift(xy_help, xy_help, BN_num_bits(N));
  BN_add(xy_help, xy_help, y);
  BN_lshift(xy_help, xy_help, BN_num_bits(N));
  
#ifdef _DEBUG
  char* tmp;
#endif
  // calculate _mu_prime
  for (unsigned long i = 1; i <= t; i++) {
    Ti /= 2L;
    if (i > d_max) {  // we have to do sequential squarings
      BN_copy(mu_prime, x);
      for (unsigned long tt = 1L; tt <= Ti; tt++) {
        BN_mod_sqr(mu_prime, mu_prime, N, ctx);
      }
    } else {  // we use the cache
      unsigned long k;
      const int bits_size = CHAR_BIT*sizeof(decltype(k));
      std::bitset<bits_size> k_bits;
      for (k = 0; k < cache[i-1].size(); k++) {
        k_bits = cache[i-1].size()-1-k;
        BN_dec2bn(&prod_help, "1");
        BN_copy(prod_help2, prod_help);
        for (long b = 0; b < i; b++) {
          if (k_bits[bits_size-i+b]) {
            BN_mul(prod_help, prod_help, r_cache[b], ctx);
          }
        }
        BN_mod_exp(prod_help, cache[i-1][k], prod_help, N, ctx);
        BN_mod_mul(prod_help2, prod_help2, prod_help, N, ctx);
      }
      BN_copy(mu_prime, prod_help2);
    }
    _mu_prime[i-1].resize(BN_num_bytes(mu_prime));
    BN_bn2bin(mu_prime, &(_mu_prime[i-1][0]));
    BN_mod_sqr(mu, mu_prime, N, ctx);
    BN_copy(xymu, xy_help);
    BN_add(xymu, xymu, mu_prime);
    hash(xymu, r);
    if (i <= d_max) {
      BN_copy(r_cache[i-1], r);
    }
    // get the new x
    BN_mod_exp(prod_help, x, r, N, ctx);
    BN_mod_mul(x, prod_help, mu, N, ctx);
#ifdef _DEBUG
    tmp = BN_bn2dec(x);
    std::cout << i << ":\t" << tmp << std::endl;
    OPENSSL_free(tmp);
#endif
    // get the new y
    BN_mod_exp(prod_help, mu, r, N, ctx);
    BN_mod_mul(y, prod_help, y, N, ctx);
#ifdef _DEBUG
    tmp = BN_bn2dec(y);
    std::cout << i << ":\t" << tmp << std::endl << std::endl;
    OPENSSL_free(tmp);
#endif
  }
  
#ifdef _DEBUG
  std::cout << std::endl << "-----------------------------------------------------------" << std::endl;
#endif

  BN_CTX_end(ctx);
  return std::make_pair(_mu_prime, _y);
}
