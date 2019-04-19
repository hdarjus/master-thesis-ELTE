#include "../include/ProverPietrzak.h"
#include "../include/util.h"
#include <openssl/err.h>
#include <bitset>

ProverPietrzak::ProverPietrzak() :
    ctx_ptr(BN_CTX_free_ptr(BN_CTX_secure_new(), ::BN_CTX_free)) { }

typename ProverPietrzak::solution
ProverPietrzak::operator()(const VerifierPietrzak& verifier, long _d_max) const {
  BN_CTX* ctx = ctx_ptr.get();
  BN_CTX_start(ctx);

  // constants
  const auto puzzle = verifier.get_RSWPuzzle();
  const auto hash = verifier.get_Hash();
  const unsigned long t = puzzle.get_log2T();
  const bytevec _T = puzzle.get_T();
  const bytevec _N = puzzle.get_N();
  const bytevec _x = puzzle.get_x();

  // helper variables
  const unsigned long d_max = (_d_max >= 0) ? (unsigned long)_d_max : t/2;
  if (d_max > t) {
    throw std::runtime_error("d_max is too large");
  }
  BIGNUM* mu = BN_CTX_get(ctx);
  BIGNUM* N = BN_CTX_get(ctx);
  BIGNUM* T = BN_CTX_get(ctx);
  BIGNUM* Tip1 = BN_CTX_get(ctx);  // denotes T_{i+1} from the paper
  BIGNUM* x = BN_CTX_get(ctx);
  BIGNUM* r = BN_CTX_get(ctx);
  BIGNUM* y = BN_CTX_get(ctx);
  BIGNUM* tt = BN_CTX_get(ctx);
  BIGNUM* xy_help = BN_CTX_get(ctx);
  BIGNUM* xymu = BN_CTX_get(ctx);
  BIGNUM* prod_help = BN_CTX_get(ctx);
  BIGNUM* prod_help2 = BN_CTX_get(ctx);
  BIGNUM* two_to_d = BN_CTX_get(ctx);
  BIGNUM* q = BN_CTX_get(ctx);
  BIGNUM* rem = BN_CTX_get(ctx);
  std::vector<std::vector<BIGNUM*> > cache(d_max);
  std::vector<BIGNUM*> r_cache(d_max);
  for (int i = 1; i <= d_max; i++) {
    r_cache[i-1] = BN_CTX_get(ctx);
    cache[i-1].resize(exp2(i-1));
    for (int k = 0; k < cache[i-1].size(); k++) {
      cache[i-1][k] = BN_CTX_get(ctx);
    }
  }
  const unsigned long err = ERR_get_error();
  if (err != 0) {
    char* err_char = new char[300];
    ERR_error_string_n(err, err_char, 300);
    std::string err_string(err_char);
    delete[] err_char;
    throw std::runtime_error(err_string);
  }

  // set initial values
  BN_bin2bn(_x.data(), (int)_x.size(), x);
  BN_bin2bn(_N.data(), (int)_N.size(), N);
  BN_bin2bn(_T.data(), (int)_T.size(), T);
  BN_copy(Tip1, T);

  // declare returned values
  bytevec _y;
  std::vector<bytevec> _mu(t);

  // calculate _y and save partial results
  BN_copy(y, x);
  for (BN_one(tt); BN_cmp(tt, T) <= 0;  BN_add(tt, tt, BN_value_one())) {
    BN_mod_sqr(y, y, N, ctx);
    bool saved = false;
    for (unsigned long d_ind = 0; d_ind < d_max && !saved; d_ind++) {
      BN_one(two_to_d);
      BN_lshift(two_to_d, two_to_d, t-d_ind-1);
      BN_div(q, rem, tt, two_to_d, ctx);
      if (BN_is_zero(rem) && BN_cmp(tt, T) < 0) {
        const auto q_word = BN_get_word(q);
        BN_copy(cache[d_ind][(q_word-1)/2], y);
        saved = true;
      }
    }
  }
  bn2bytevec(y, _y);

  // calculate xy_help that helps the hashing step
  BN_copy(xy_help, x);
  BN_lshift(xy_help, xy_help, BN_num_bits(N));
  BN_add(xy_help, xy_help, y);
  BN_lshift(xy_help, xy_help, BN_num_bits(N));
  
  // calculate _mu
  for (unsigned long i = 1; i <= t; i++) {
    BN_rshift1(Tip1, Tip1);
    if (i > d_max) {  // we have to do sequential squarings
      BN_copy(mu, x);
      for (BN_one(tt); BN_cmp(tt, Tip1) <= 0; BN_add(tt, tt, BN_value_one())) {
        BN_mod_sqr(mu, mu, N, ctx);
      }
    } else {  // we use the cache
      BN_one(prod_help2);
      unsigned long k;
      const int bits_size = CHAR_BIT*sizeof(decltype(k));
      std::bitset<bits_size> k_bits;
      for (k = 0; k < cache[i-1].size(); k++) {
        k_bits = cache[i-1].size()-1-k;
        BN_one(prod_help);
        for (unsigned long b = 0; b < i-1; b++) {
          if (k_bits.test(i-2-b)) {
            BN_mul(prod_help, prod_help, r_cache[b], ctx);
          }
        }
        BN_mod_exp(prod_help, cache[i-1][k], prod_help, N, ctx);
        BN_mod_mul(prod_help2, prod_help2, prod_help, N, ctx);
      }
      BN_copy(mu, prod_help2);
    }
    bn2bytevec(mu, _mu[i-1]);
#ifdef _DEBUG
      std::cout << "mu:\t" << print_bn_hex(mu) << std::endl;
#endif
    BN_copy(xymu, xy_help);
    BN_add(xymu, xymu, mu);
#ifdef _DEBUG
      std::cout << "xymu:\t" << print_bn_hex(xymu) << std::endl;
#endif
    hash(xymu, r);
#ifdef _DEBUG
      std::cout << "r:\t" << print_bn(r) << std::endl;
#endif
    if (i <= d_max) {
      BN_copy(r_cache[i-1], r);
    }
    // get the new x
    BN_mod_exp(prod_help, x, r, N, ctx);
    BN_mod_mul(x, prod_help, mu, N, ctx);
#ifdef _DEBUG
      std::cout << "x:\t" << print_bn(x) << std::endl;
#endif
    // get the new y
    BN_mod_exp(prod_help, mu, r, N, ctx);
    BN_mod_mul(y, prod_help, y, N, ctx);
#ifdef _DEBUG
      std::cout << "y:\t" << print_bn(y) << std::endl;
      std::cout << "----------------" << std::endl << std::endl;
#endif
  }
  
#ifdef _DEBUG
  std::cout << std::endl << "-----------------------------------------------------------" << std::endl << std::endl;
#endif

  BN_CTX_end(ctx);
  return std::make_pair(_mu, _y);
}
