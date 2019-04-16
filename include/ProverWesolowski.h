#pragma once

#include <openssl/bn.h>
#include <cmath>
#include "VerifierWesolowski.h"

class ProverWesolowski {
public:
  using solution = typename VerifierWesolowski::solution;

  ProverWesolowski();
  ~ProverWesolowski() = default;

  solution operator()(const VerifierWesolowski& verifier) const;

private:
  BN_CTX_free_ptr ctx_ptr;
};

