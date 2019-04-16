#pragma once

#include "types.h"
#include "Hash.h"
#include "RSWPuzzle.h"
#include <cmath>
#include "util.h"

#include <iostream>

class VerifierWesolowski {
public:
  using solution = ::Solution<bytevec>;

  VerifierWesolowski (
      const unsigned long _lambda,
      const unsigned long _T,
      const bytevec& _x,
      const unsigned long _lambdaRSW,
      const unsigned int _key_size = 256/Hash::divisor,
      const unsigned int _block_size = 128/Hash::divisor);
  ~VerifierWesolowski() = default;

  Hash get_Hash () const;
  RSWPuzzle get_RSWPuzzle () const;
  bool operator()(const solution& sol) const;

private:
  BN_CTX_free_ptr ctx_ptr;
  Hash hash;
  RSWPuzzle puzzle;
};

