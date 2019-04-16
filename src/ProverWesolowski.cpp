#include "../include/ProverWesolowski.h"

ProverWesolowski::ProverWesolowski() :
    ctx_ptr(BN_CTX_free_ptr(BN_CTX_secure_new(), ::BN_CTX_free)) { }

typename ProverWesolowski::solution
ProverWesolowski::operator()(const VerifierWesolowski& verifier) const {
  throw std::runtime_error("nyi");
}

