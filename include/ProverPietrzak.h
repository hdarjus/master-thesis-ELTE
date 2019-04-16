#pragma once

#include <openssl/bn.h>
#include <cmath>
#include "VerifierPietrzak.h"

class ProverPietrzak {
public:
  using solution = typename VerifierPietrzak::solution;

  ProverPietrzak();
  ~ProverPietrzak() = default;

  solution operator()(const VerifierPietrzak& verifier) const;

private:
  BN_CTX_free_ptr ctx_ptr;
};

