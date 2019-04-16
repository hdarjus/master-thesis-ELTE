#pragma once

#include "types.h"
#include "Hash.h"
#include "RSWPuzzle.h"
#include <cmath>
#include "util.h"

#include <iostream>

template<VDF_version m>
class Verifier {
public:
  using solution = bool;
};

template<>
class Verifier<VDF_version::PIETRZAK> {
public:
  using solution = ::solution<std::vector<bytevec> >;

  Verifier(  // proof of concept
      const unsigned long _lambda,
      const unsigned long _T,
      const bytevec& _x,
      const unsigned long _lambdaRSW,
      const unsigned int _key_size = 256/Hash::divisor,
      const unsigned int _block_size = 128/Hash::divisor) :
        hash(_lambda, _key_size, _block_size),
        puzzle(_lambda, _T, _x, _lambdaRSW) {
    ctx = BN_CTX_secure_new();
  }

  ~Verifier() {
    BN_CTX_free(ctx);
  }

  Hash get_Hash () const {
    return hash;
  }

  RSWPuzzle get_RSWPuzzle () const {
    return puzzle;
  }

  bool operator()(const solution& sol) const {
    BN_CTX_start(ctx);

    // constants
    const unsigned long lambda = puzzle.get_lambda();
    const unsigned long T = puzzle.get_T();
    const bytevec _N = puzzle.get_N();
    const bytevec _x = puzzle.get_x();
    const std::vector<bytevec> _pi = sol.first;
    const bytevec _y = sol.second;

    // helper variables
    unsigned long Ti;  // denotes T_{i+1} from the paper
    unsigned long t = std::floor(std::log2(T));
    BIGNUM* mu = BN_CTX_get(ctx);
    BIGNUM* mu_prime = BN_CTX_get(ctx);
    BIGNUM* N = BN_CTX_get(ctx);
    BIGNUM* r = BN_CTX_get(ctx);
    BIGNUM* x = BN_CTX_get(ctx);
    BIGNUM* y = BN_CTX_get(ctx);
    BIGNUM* xy_help = BN_CTX_get(ctx);
    BIGNUM* xymu = BN_CTX_get(ctx);
    BIGNUM* prod_help = BN_CTX_get(ctx);
    BIGNUM* zero = BN_CTX_get(ctx);

    // set initial values
    Ti = T;
    BN_bin2bn(_x.data(), (int)_x.size(), x);
    BN_bin2bn(_N.data(), (int)_N.size(), N);
    BN_bin2bn(_y.data(), (int)_y.size(), y);
    BN_zero(zero);

#ifdef _DEBUG
      std::cout << "N:\t" << print_bn(N) << std::endl;
#endif

    // calculate xy_help that helps the hashing step
    BN_copy(xy_help, x);
    BN_lshift(xy_help, xy_help, BN_num_bits(N));
    BN_add(xy_help, xy_help, y);
    BN_lshift(xy_help, xy_help, BN_num_bits(N));

    // validation
    for (int i = 1; i <= _pi.size(); i++) {
        std::cout << i << std::endl;
      Ti /= 2L;
      BN_bin2bn(_pi[i-1].data(), (int)_pi[i-1].size(), mu_prime);
#ifdef _DEBUG
        std::cout << "mu':\t" << print_bn_hex(mu_prime) << std::endl;
#endif
      if (BN_cmp(mu_prime, N) >= 0 || BN_cmp(mu_prime, zero) != 1) {  // 0 < mu_prime < N?
        return false;
      }
      BN_mod_sqr(mu, mu_prime, N, ctx);
#ifdef _DEBUG
        std::cout << "mu:\t" << print_bn_hex(mu) << std::endl;
#endif
      BN_copy(xymu, xy_help);
      BN_add(xymu, xymu, mu_prime);
#ifdef _DEBUG
        std::cout << "xymu:\t" << print_bn_hex(xymu) << std::endl;
#endif
      hash(xymu, r);
#ifdef _DEBUG
        std::cout << "r:\t" << print_bn(r) << std::endl;
#endif
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
    std::cout << std::endl << "-----------------------------------------------------------------" << std::endl;
#endif

    BN_mod_sqr(x, x, N, ctx);
#ifdef _DEBUG
      std::cout << "x:\t" << print_bn(x) << std::endl;
      std::cout << "y:\t" << print_bn(y) << std::endl;
#endif
    bool result = BN_cmp(y, x) == 0;
    BN_CTX_end(ctx);
    return result;
  }

private:
  Hash hash;
  RSWPuzzle puzzle;
  BN_CTX* ctx;
};

template<>
class Verifier<VDF_version::WESOLOWSKI> {
public:
  using solution = ::solution<bytevec>;

  Verifier(  // proof of concept
      const unsigned long _lambda,
      const unsigned long _T,
      const bytevec& _x,
      const unsigned long _lambdaRSW,
      const unsigned int _key_size = 256/Hash::divisor,
      const unsigned int _block_size = 128/Hash::divisor) :
        hash(_lambda, _key_size, _block_size),
        puzzle(_lambda, _T, _x, _lambdaRSW) { }
  ~Verifier() = default;

  Hash get_Hash () const {
    return hash;
  }

  RSWPuzzle get_RSWPuzzle () const {
    return puzzle;
  }

  bool operator()(const solution& sol) const {
    return true;  // TODO
  }

private:
  Hash hash;
  RSWPuzzle puzzle;
};

