#include "../include/RSWPuzzle.h"

RSWPuzzle::RSWPuzzle (const BIGNUM* N, const BIGNUM* x, const BIGNUM* T): _N(BN_dup(N)), _x(BN_dup(x)), _T(BN_dup(T)) { }

RSWPuzzle::RSWPuzzle (BN_CTX* ctx, const int lambda, const BIGNUM* x, const BIGNUM* T): _x(BN_dup(x)), _T(BN_dup(T)) {
  _N = BN_new();
  BIGNUM *p, *q;
  BN_CTX_start(ctx);
  p = BN_CTX_get(ctx);
  q = BN_CTX_get(ctx);

  BN_generate_prime_ex(p, lambda+3, 1, NULL, NULL, NULL);
  BN_generate_prime_ex(q, lambda+5, 1, NULL, NULL, NULL);
  BN_mul(_N, p, q, ctx);

  BN_CTX_end(ctx);
}

RSWPuzzle::~RSWPuzzle () {
  BN_free(_N);
  BN_free(_x);
  BN_free(_T);
}

const BIGNUM* RSWPuzzle::N() const {
  return _N;
}

const BIGNUM* RSWPuzzle::x() const {
  return _x;
}

const BIGNUM* RSWPuzzle::T() const {
  return _T;
}

