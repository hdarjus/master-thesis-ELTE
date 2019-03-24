#ifndef _RSW_PUZZLE_H_
#define _RSW_PUZZLE_H_

#include <string>
#include "types.h"
#include <openssl/bn.h>
#include <openssl/rand.h>

class RSWPuzzle {
public:
  RSWPuzzle(const unsigned long _lambda, const unsigned long _T, const bytevec& _x, const bytevec& _N);
  RSWPuzzle(const unsigned long _lambda, const unsigned long _T, const bytevec& _x, const unsigned long _lambdaRSW);
  ~RSWPuzzle();

  bytevec get_N() const;
  bytevec get_x() const;
  unsigned long get_T() const;
  unsigned long get_lambda() const;
  
protected:
  RSWPuzzle(const unsigned long _lambda, const unsigned long _T, const bytevec& _x);
  BIGNUM* N;
  BIGNUM* x;
  const unsigned long T;
  const unsigned long lambda;
};

#endif  // _RSWPUZZLE_H_
