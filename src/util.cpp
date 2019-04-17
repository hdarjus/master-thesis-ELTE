#include "../include/util.h"
#include <sstream>

bytevec bn2bytevec(const BIGNUM* in) {
  bytevec vec(BN_num_bytes(in));
  BN_bn2bin(in, &vec[0]);
  return vec;
}

void bn2bytevec(const BIGNUM* in, bytevec& out) {
  out.resize(BN_num_bytes(in));
  BN_bn2bin(in, &out[0]);
}

std::string print_bytevec (const bytevec& b) {
  std::ostringstream result;
  char* str;
  BIGNUM* bn = BN_new();
  BN_bin2bn(b.data(), b.size(), bn);
  str = BN_bn2dec(bn);
  result << str;
  BN_free(bn);
  OPENSSL_free(str);
  return result.str();
}

std::string print_bn(const BIGNUM* bn) {
  std::ostringstream result;
  char* str;
  str = BN_bn2dec(bn);
  result << str;
  OPENSSL_free(str);
  return result.str();
}

std::string print_bn_hex(const BIGNUM* bn) {
  std::ostringstream result;
  char* str;
  str = BN_bn2hex(bn);
  result << str;
  OPENSSL_free(str);
  return result.str();
}
