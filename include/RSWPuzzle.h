#ifndef _RSW_PUZZLE_H_
#define _RSW_PUZZLE_H_

#include <openssl/bn.h>
#include <openssl/rand.h>

class RSWPuzzle {
public:
  RSWPuzzle (const BIGNUM* _N, const int _lambda, const BIGNUM* _x, const BIGNUM* _T);
  RSWPuzzle (const BN_CTX* ctx, const int _lambda, const BIGNUM* _x, const BIGNUM* _T);
  ~RSWPuzzle ();

  const BIGNUM* get_N() const;
  const BIGNUM* get_x() const;
  const BIGNUM* get_T() const;
  const int get_lambda() const;
  
private:
  BIGNUM* N;
  BIGNUM* x;
  BIGNUM* T;
  const int lambda;
};

#endif  // _RSWPUZZLE_H_
