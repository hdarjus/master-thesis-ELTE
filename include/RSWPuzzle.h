#ifndef _RSW_PUZZLE_H_
#define _RSW_PUZZLE_H_

extern "C" {
#include <openssl/bn.h>
#include <openssl/rand.h>
}

class RSWPuzzle {
public:
  RSWPuzzle (const BIGNUM* N, const BIGNUM* x, const BIGNUM* T);
  RSWPuzzle (BN_CTX* ctx, const int lambda, const BIGNUM* x, const BIGNUM* T);
  ~RSWPuzzle ();

  const BIGNUM* N() const;
  const BIGNUM* x() const;
  const BIGNUM* T() const;
  
private:
  BIGNUM* _N;
  BIGNUM* _x;
  BIGNUM* _T;
};

#endif  // _RSWPUZZLE_H_
