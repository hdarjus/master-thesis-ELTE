extern "C" {
#include <stdio.h>
#include <stdarg.h>
}
#include "../include/Hash2Prime.h"
#include "../include/util.h"
#include <algorithm>


using namespace std;

Hash2Prime::Hash2Prime(const int& lambda) :
    primeGen(gmp_randinit_mt) {
  expmLambertWm1(lambda);
  // divide max_int by 6 to take only values 6k+-1 as candidates
  mpz_cdiv_q(max_int.get_mpz_t(), max_int.get_mpz_t(), (6_mpz).get_mpz_t());
}

void Hash2Prime::operator()(const BIGNUM* in, BIGNUM* out) {
  const int max_iter = 10000;
  const int max_miller_rabin = 30;

  bn2bytevec(in, cache);
  mpz_import(seed.get_mpz_t(), cache.size(), 1, sizeof(cache[0]), 1, 0, cache.data());
  primeGen.seed(seed);

  int isprime = 0;
  int count = 0;
  while (isprime <= 0 && count < max_iter) {
    candidate = primeGen.get_z_range(max_int);
    sign = primeGen.get_z_bits(1);
    sign = 2*sign-1;
    candidate = 6*candidate+sign;
    isprime = mpz_probab_prime_p(candidate.get_mpz_t(), max_miller_rabin);
    count++;
  }

  if (count == max_iter)
    throw runtime_error("Prime not found!");

  cache.resize(2+mpz_sizeinbase(candidate.get_mpz_t(), 2)/CHAR_BIT);
  size_t countp = 0;
  mpz_export(&cache[0], &countp, 1, sizeof(cache[0]), 1, 0, candidate.get_mpz_t());
  cache.resize(countp);
  BN_bin2bn(cache.data(), cache.size(), out);
  return;
}

// ideas taken from https://github.com/BrianGladman/gsl/blob/9caa2b8fff209fde55f28d1cbe0bab7f94044559/specfunc/lambert.c#L207
// calculates ceil(exp(-W_{-1}(-2^{-\lambda}))), where
//   -W_{-1} is Lambert's W function at branch -1
void Hash2Prime::expmLambertWm1(const int& lambda) {
  // init variables
  const unsigned int precision = 4096u;
  const mpfr_rnd_t rounding = MPFR_RNDN;
  mpfr_t L1, L2, w, x, eps, tol, e, p, t, tmp, tmp2;
  mpfr_inits2(precision, L1, L2, w, x, eps, tol, e, p, t, tmp, tmp2, NULL);
  mpf_class ww(0, precision);
  const int maxiter = 32;

  mpfr_set_si_2exp(x, 1, -lambda, rounding);
  mpfr_set(tmp, x, rounding);
  mpfr_neg(x, x, rounding);
  mpfr_log(L1, tmp, rounding);
  mpfr_neg(tmp, L1, rounding);
  mpfr_log(L2, tmp, rounding);

  mpfr_div(tmp, L2, L1, rounding);  // w = L1-L2+L2/L1;
  mpfr_sub(tmp, tmp, L2, rounding);
  mpfr_add(w, tmp, L1, rounding);
  mpfr_set_si_2exp(eps, 1, -std::min(200l, precision/3l), rounding);

  bool found = false;
  for (int i = 0; i < maxiter && !found; i++) {
    // t = exp(w)*w-x
    mpfr_exp(e, w, rounding);
    mpfr_mul(tmp, w, e, rounding);
    mpfr_sub(t, tmp, x, rounding);

    mpfr_add_si(p, w, 1, rounding);
    if (mpfr_sgn(w) > 0) {  // Newton iteration
      // t = (t/p)/e
      mpfr_div(tmp, t, p, rounding);
      mpfr_div(t, tmp, e, rounding);
    } else {  // Halley iteration
      // t /= e*p - 0.5*(p + 1.0)*t/p
      mpfr_mul(tmp2, e, p, rounding);
      mpfr_add_si(tmp, p, 1, rounding);
      mpfr_mul_d(tmp, tmp, 0.5, rounding);
      mpfr_mul(tmp, tmp, t, rounding);
      mpfr_div(tmp, tmp, p, rounding);
      mpfr_sub(tmp, tmp2, tmp, rounding);
      mpfr_div(t, t, tmp, rounding);
    }
    // w -= t
    mpfr_sub(w, w, t, rounding);
    
    // calculate tolerance
    // 10*eps*max(|w|, 1/(|p|*e))
    mpfr_abs(tmp, p, rounding);
    mpfr_mul(tmp, tmp, e, rounding);
    mpfr_d_div(tmp, 1, tmp, rounding);
    mpfr_abs(tmp2, w, rounding);
    mpfr_max(tmp, tmp, tmp2, rounding);
    mpfr_mul_si(tmp, tmp, 10, rounding);
    mpfr_mul(tol, eps, tmp, rounding);
    
    // if the change is smaller than the tolerance we've found the solution
    mpfr_abs(tmp, t, rounding);
    if (mpfr_cmp(tol, tmp) > 0) {
      found = true;
    }
  }

  // calculate ceil(exp(-w))
  mpfr_neg(w, w, rounding);
  mpfr_exp(w, w, rounding);
  mpfr_get_f(ww.get_mpf_t(), w, rounding);
  max_int = ceil(ww);

  // free up space
  mpfr_free_cache();
  mpfr_clears(L1, L2, w, x, eps, tol, e, p, t, tmp, tmp2, NULL);
  
  return;
}

