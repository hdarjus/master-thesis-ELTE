#pragma once

#include <openssl/bn.h>
#include <openssl/rand.h>
#include "types.h"
#include <string>

class RSWPuzzle {
public:
  RSWPuzzle(const unsigned long _lambda, const unsigned long _T, const bytevec& _x, const bytevec& _N);
  RSWPuzzle(const unsigned long _lambda, const unsigned long _T, const bytevec& _x, const unsigned long _lambdaRSW);
  RSWPuzzle(const RSWPuzzle& other);
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

