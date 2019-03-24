#ifndef _VERIFIER_HPP_
#define _VERIFIER_HPP_

#include "types.h"
#include "Hash.h"
#include "RSWPuzzle.h"

template<VDF_version m>
class Verifier {
public:
  using solution = int;
};

template<>
class Verifier<VDF_version::PIETRZAK> {
public:
  using solution = ::solution<std::vector<bytevec> >;

  Verifier(  // proof of concept
      const unsigned long _lambda,
      const unsigned long _T,
      const bytevec& _x,
      const unsigned long _lambdaRSW,
      const unsigned int _key_size = 256/Hash::divisor,
      const unsigned int _block_size = 128/Hash::divisor) :
        hash(_lambda, _key_size, _block_size),
        puzzle(_lambda, _T, _x, _lambdaRSW) { }
  ~Verifier() = default;

  bool verify(const solution& sol) const {
    return true;  // TODO
  }

private:
  Hash hash;
  RSWPuzzle puzzle;
};

template<>
class Verifier<VDF_version::WESOLOWSKI> {
public:
  using solution = ::solution<bytevec>;

  Verifier(  // proof of concept
      const unsigned long _lambda,
      const unsigned long _T,
      const bytevec& _x,
      const unsigned long _lambdaRSW,
      const unsigned int _key_size = 256/Hash::divisor,
      const unsigned int _block_size = 128/Hash::divisor) :
        hash(_lambda, _key_size, _block_size),
        puzzle(_lambda, _T, _x, _lambdaRSW) { }
  ~Verifier() = default;

  bool verify(const solution& sol) const {
    return true;  // TODO
  }

private:
  Hash hash;
  RSWPuzzle puzzle;
};

#endif  // _VERIFIER_HPP_

