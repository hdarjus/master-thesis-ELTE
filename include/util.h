#pragma once

#include <openssl/bn.h>
#include <string>
#include "../include/types.h"

std::string print_bn(const BIGNUM* bn);
std::string print_bn_hex(const BIGNUM* bn);
std::string print_bytevec (const bytevec& b);

