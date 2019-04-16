#include "../include/VerifierWesolowski.h"

VerifierWesolowski::VerifierWesolowski(
    const unsigned long _lambda,
    const unsigned long _T,
    const bytevec& _x,
    const unsigned long _lambdaRSW,
    const unsigned int _key_size,
    const unsigned int _block_size) :
      hash(_lambda, _key_size, _block_size),
      puzzle(_lambda, _T, _x, _lambdaRSW),
      ctx_ptr(BN_CTX_free_ptr(BN_CTX_secure_new(), ::BN_CTX_free)) { }

Hash VerifierWesolowski::get_Hash () const {
  return hash;
}

RSWPuzzle VerifierWesolowski::get_RSWPuzzle () const {
  return puzzle;
}

bool VerifierWesolowski::operator()(const solution& sol) const {
  throw std::runtime_error("nyi");
}

