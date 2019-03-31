#include "../include/RSWPuzzle.h"

RSWPuzzle::RSWPuzzle(
    const unsigned long _lambda,
    const unsigned long _T,
    const bytevec& _x) :
      lambda(_lambda), T(_T) {
  x = BN_new();

  BN_bin2bn(_x.data(), _x.size(), x);
}

RSWPuzzle::RSWPuzzle(
    const unsigned long _lambda,
    const unsigned long _T,
    const bytevec& _x,
    const bytevec& _N) :
      RSWPuzzle(_lambda, _T, _x) {
  N = BN_new();
  BN_bin2bn(_N.data(), _N.size(), N);

  BN_CTX* ctx = BN_CTX_new();
  BN_CTX_start(ctx);
  BN_mod(x, x, N, ctx);
  BN_CTX_end(ctx);
  BN_CTX_free(ctx);
}

RSWPuzzle::RSWPuzzle(
    const unsigned long _lambda,
    const unsigned long _T,
    const bytevec& _x,
    const unsigned long _lambdaRSW) :
      RSWPuzzle(_lambda, _T, _x) {
  N = BN_new();

  BN_CTX* ctx = BN_CTX_new();
  BN_CTX_start(ctx);
  BIGNUM* p, * q;
  p = BN_CTX_get(ctx);
  q = BN_CTX_get(ctx);

  BN_generate_prime_ex(p, _lambdaRSW/2, 1, NULL, NULL, NULL);
  BN_generate_prime_ex(q, _lambdaRSW/2, 1, NULL, NULL, NULL);
  BN_mul(N, p, q, ctx);

  BN_mod(x, x, N, ctx);

  BN_CTX_end(ctx);
  BN_CTX_free(ctx);
}

RSWPuzzle::RSWPuzzle(const RSWPuzzle& other) :
    T(other.T), lambda(other.lambda) {
  N = BN_dup(other.N);
  x = BN_dup(other.x);
}

RSWPuzzle::~RSWPuzzle() {
  BN_free(x);
  BN_free(N);
}

bytevec bn2bytevec(const BIGNUM* in) {
  bytevec vec(BN_num_bytes(in));
  BN_bn2bin(in, &vec[0]);
  return vec;
}

bytevec RSWPuzzle::get_N() const {
  return bn2bytevec(N);
}

bytevec RSWPuzzle::get_x() const {
  return bn2bytevec(x);
}

unsigned long RSWPuzzle::get_T() const {
  return T;
}

unsigned long RSWPuzzle::get_lambda() const {
  return lambda;
}

