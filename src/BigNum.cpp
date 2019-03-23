extern "C" {
#include <openssl/bn.h>
}

#include <string>
#include <stdexcept>
#include "../include/BigNum.h"

using namespace std;

void to_bn (BIGNUM* bn, const string s) {
  BN_dec2bn(&bn, s.c_str());
}

void to_bn (BIGNUM* bn, const int n) {
  to_bn(bn, to_string(n));
}

BigNum::BigNum () {
  value = BN_new();
  to_bn(value, 0);
}

BigNum::BigNum (const BigNum& rhs) {
  value = BN_dup(rhs.value);
}

BigNum& BigNum::operator= (const BigNum& rhs) {
  if (this != &rhs)
    value = BN_dup(rhs.value);
  return *this;
}

const BigNum BigNum::operator+ (const BigNum& rhs) const {
  return BigNum(*this) += rhs;
}

const BigNum BigNum::operator- (const BigNum& rhs) const {
  return BigNum(*this) -= rhs;
}

const BigNum BigNum::operator* (const BigNum& rhs) const {
  return BigNum(*this) *= rhs;
}

const BigNum BigNum::operator/ (const BigNum& rhs) const {
  return BigNum(*this) /= rhs;
}

const BigNum BigNum::operator% (const BigNum& rhs) const {
  return BigNum(*this) %= rhs;
}

BigNum& BigNum::operator+= (const BigNum& rhs) {
  throw runtime_error("nyi");
}

BigNum::~BigNum () {
  BN_free(value);
}

string BigNum::print () const {
  return BN_bn2dec(value);
}

