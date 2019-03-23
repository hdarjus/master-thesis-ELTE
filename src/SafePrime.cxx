extern "C" {
#include <openssl/bn.h>
#include <openssl/rand.h>
}

#include <string>
#include <stdexcept>
#include "../include/SafePrime.h"

using namespace std;

SafePrime::SafePrime () {
  value = BN_new();
  str = "";
}

SafePrime::~SafePrime () {
  BN_free(value);
}

void SafePrime::generate (const int size) {
  if (!BN_generate_prime_ex(value, size, 1, NULL, NULL, NULL))
    throw runtime_error("Could not generate prime");
  str = BN_bn2dec(value);
  return;
}

const string SafePrime::print () const {
  if (str == "")
    throw runtime_error("No value yet");
  return str;
}

void SafePrime::seed (const string buffer) {
  RAND_seed(buffer.c_str(), sizeof buffer.c_str());
  return;
}

