#pragma once

#include <openssl/bn.h>
#include <cmath>
#include "Verifier.hpp"

template<VDF_version m>
class Prover {
public:
  using solution = typename Verifier<m>::solution;

  Prover() {
    ctx = BN_CTX_new();
  }

  ~Prover() {
    BN_CTX_free(ctx);
  }

  solution operator()(const Verifier<m>& verifier) const;

private:
  BN_CTX* ctx;
};

using ProverP = Prover<VDF_version::PIETRZAK>;
using ProverW = Prover<VDF_version::WESOLOWSKI>;

template<>
typename ProverP::solution
ProverP::operator()(const Verifier<VDF_version::PIETRZAK>& verifier) const {
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

  // set initial values
  Ti = T;
  BN_bin2bn(_x.data(), (int)_x.size(), x);
  BN_bin2bn(_N.data(), (int)_N.size(), N);
  BN_copy(y, x);

  // declare returned values
  bytevec _y;
  std::vector<bytevec> _mu_prime(t);

  // calculate _y and save the result halfway for the proof
  for (unsigned long tt = 1; tt <= T; tt++) {
    BN_mod_sqr(y, y, N, ctx);
    if (tt == T/2L-1L) {  // maybe save later more but it's painful and error prone
      BN_copy(mu_prime, y);
    }
  }
  _y.resize(BN_num_bytes(y));
  BN_bn2bin(y, &_y[0]);

  // calculate xy_help that helps the hashing step
  BN_copy(xy_help, x);
  BN_lshift(xy_help, xy_help, BN_num_bits(N));
  BN_add(xy_help, xy_help, y);
  BN_lshift(xy_help, xy_help, BN_num_bits(N));
  
  char* tmp;
  // calculate _mu_prime
  for (int i = 1; i <= t; i++) {
    Ti /= 2L;
    if (i > 1) {  // i == 1: we already know mu_prime
      BN_copy(mu_prime, x);
      for (unsigned long tt = 1L; tt+1L <= Ti; tt++) {
        BN_mod_sqr(mu_prime, mu_prime, N, ctx);
      }
    }
    _mu_prime[i-1].resize(BN_num_bytes(mu_prime));
    BN_bn2bin(mu_prime, &(_mu_prime[i-1][0]));
    BN_mod_sqr(mu, mu_prime, N, ctx);
    BN_copy(xymu, xy_help);
    BN_add(xymu, xymu, mu_prime);
    hash(xymu, r);
    // get the new x
    BN_mod_exp(prod_help, x, r, N, ctx);
    BN_mod_mul(x, prod_help, mu, N, ctx);
    tmp = BN_bn2dec(x);
    std::cout << i << ":\t" << tmp << std::endl;
    OPENSSL_free(tmp);
    // get the new y
    BN_mod_exp(prod_help, mu, r, N, ctx);
    BN_mod_mul(y, prod_help, y, N, ctx);
    tmp = BN_bn2dec(y);
    std::cout << i << ":\t" << tmp << std::endl << std::endl;
    OPENSSL_free(tmp);
  }
  
  std::cout << std::endl << "-----------------------------------------------------------" << std::endl;

  BN_CTX_end(ctx);
  return std::make_pair(_mu_prime, _y);
}

template<>
typename ProverW::solution
ProverW::operator()(const Verifier<VDF_version::WESOLOWSKI>& verifier) const {  // TODO
  throw std::runtime_error("nyi");
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

  // set initial values
  Ti = T;
  BN_bin2bn(_x.data(), (int)_x.size(), x);
  BN_bin2bn(_N.data(), (int)_N.size(), N);
  BN_copy(y, x);

  // declare returned values
  bytevec _y, _pi;
  std::vector<bytevec> _mu_prime(t-3);

  // calculate _y and save the result halfway for the proof
  for (unsigned long tt = 1; tt <= T; tt++) {
    BN_mod_sqr(y, y, N, ctx);
    if (tt == T/2L-1L) {  // maybe save later more but it's painful and error prone
      BN_copy(mu_prime, y);
    }
  }
  _y.resize(BN_num_bytes(y));
  BN_bn2bin(y, &_y[0]);

  // calculate xy_help that helps the hashing step
  BN_copy(xy_help, x);
  BN_lshift(xy_help, xy_help, BN_num_bits(N));
  BN_add(xy_help, xy_help, y);
  BN_lshift(xy_help, xy_help, BN_num_bits(N));
  
  // calculate _mu_prime
  for (int i = 1; i <= t-3; i++) {
    Ti /= 2L;
    if (i > 1) {  // i == 1: we already know mu_prime
      BN_copy(mu_prime, x);
      for (unsigned long tt = 1L; tt+1L <= Ti; tt++) {
        BN_mod_sqr(mu_prime, mu_prime, N, ctx);
      }
    }
    _mu_prime[i-1].resize(BN_num_bytes(mu_prime));
    BN_bn2bin(mu_prime, &(_mu_prime[i-1][0]));
    BN_mod_sqr(mu, mu_prime, N, ctx);
    BN_copy(xymu, xy_help);
    BN_add(xymu, xymu, mu_prime);
    hash(xymu, r);
    // get the new x
    BN_mod_exp(prod_help, x, r, N, ctx);
    BN_mod_mul(x, prod_help, mu, N, ctx);
    // get the new y
    BN_mod_exp(prod_help, mu, r, N, ctx);
    BN_mod_mul(y, prod_help, y, N, ctx);
  }

  BN_CTX_end(ctx);
  return std::make_pair(_pi, _y);
}

