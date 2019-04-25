#pragma once

#include "types.h"
#include "Hash2Prime.h"
#include "RSWPuzzle.h"
#include <cmath>
#include "util.h"

#include <iostream>
#include <chrono>
#include <vector>

class VerifierWesolowski {
public:
  using solution = ::Solution<bytevec>;

  VerifierWesolowski(
      const unsigned long _lambda,
      const unsigned long _t,
      const bytevec& _x,
      const unsigned long _lambdaRSW);
  VerifierWesolowski(
      const unsigned long _lambda,
      const unsigned long _t,
      const bytevec& _x,
      const bytevec& N);
  ~VerifierWesolowski() = default;

  Hash2Prime get_Hash () const;
  RSWPuzzle get_RSWPuzzle () const;
  bool operator()(const solution& sol) const;

  mutable std::vector<typename std::chrono::high_resolution_clock::duration> durations;

private:
  mutable Hash2Prime hash;
  const RSWPuzzle puzzle;
  const unsigned long lambda;
  BN_CTX_free_ptr ctx_ptr;
};

