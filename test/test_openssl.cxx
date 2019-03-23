extern "C" {
#include <openssl/bn.h>
#include <openssl/rand.h>
}
#include <stdexcept>
#include <cstdio>

using namespace std;

int main () {
  char buf[10] = "oub91xiy1";
  RAND_seed(buf, sizeof buf);  // not necessary
  BIGNUM* p = BN_new();
  BIGNUM* q = BN_new();
  if (!BN_generate_prime_ex(p, 32, 1, NULL, NULL, NULL)) {
    BN_free(p);
    BN_free(q);
    throw runtime_error("could not generate prime p");
  }
  if (!BN_generate_prime_ex(q, 32, 1, NULL, NULL, NULL)) {
    BN_free(p);
    BN_free(q);
    throw runtime_error("could not generate prime q");
  }
  char* p_char = BN_bn2dec(p);
  char* q_char = BN_bn2dec(q);
  printf("p = %s\nq = %s\n", p_char, q_char);
  BN_free(p);
  BN_free(q);
  delete p_char;
  delete q_char;
  return 0;
}

