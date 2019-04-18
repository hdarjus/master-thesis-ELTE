extern "C" {
#include <stdio.h>
#include <stdarg.h>
}
#include <gmpxx.h>
extern "C" {
#include <mpfr.h>
}
#include <iostream>
#include <cmath>

static const unsigned int real_precision = 1028;

using namespace std;

int _expmLambertWm1(const int& lambda) {  // ceil(exp(-W_{-1}(-2^{-\lambda})))
  const int maxiter = 32;
  const double x = -exp2(-lambda);
  const double L1 = log(-x);
  const double L2 = log(-L1);
  double w = L1-L2+L2/L1;
  cout << L1 << "  " << L2 << endl << w << "  " << x << endl << endl;

  bool found = false;
  for(int i=0; i<maxiter && !found; i++) {
    double tol;
    const double e = exp(w);
    const double p = w + 1.0;
    double t = w*e - x;
    cout << e << "  " << p << endl << t << endl << endl;

    if (w > 0) {
      t = (t/p)/e;  /* Newton iteration */
    } else {
      t /= e*p - 0.5*(p + 1.0)*t/p;  /* Halley iteration */
    };

    w -= t;

    tol = 10.0 * std::numeric_limits<double>::epsilon() * fmax(fabs(w), 1.0/(fabs(p)*e));
    cout << w << "  " << tol << endl << endl;
    
    if (fabs(t) < tol) {
      found = true;
    }
  }

  if (!found) {
    std::cout << "WARNING: Lambert maxiter!" << std::endl;
  }

  return (int)ceil(exp(-w));
}

// ideas taken from https://github.com/BrianGladman/gsl/blob/9caa2b8fff209fde55f28d1cbe0bab7f94044559/specfunc/lambert.c#L207
mpz_class expmLambertWm1(const int& lambda) {  // ceil(exp(-W_{-1}(-2^{-\lambda})))
  const int maxiter = 32;
  mpfr_t L1, L2, w, x, eps, tol, e, p, t, tmp, tmp2;
  mpfr_inits2(real_precision, L1, L2, w, x, eps, tol, e, p, t, tmp, tmp2, NULL);
  mpfr_set_si_2exp(x, 1, -lambda, MPFR_RNDN);
  mpfr_set(tmp, x, MPFR_RNDN);
  mpfr_neg(x, x, MPFR_RNDN);
  mpfr_log(L1, tmp, MPFR_RNDN);
  mpfr_neg(tmp, L1, MPFR_RNDN);
  mpfr_log(L2, tmp, MPFR_RNDN);

  mpfr_div(tmp, L2, L1, MPFR_RNDN);  // w = L1-L2+L2/L1;
  mpfr_sub(tmp, tmp, L2, MPFR_RNDN);
  mpfr_add(w, tmp, L1, MPFR_RNDN);
  mpfr_set_si_2exp(eps, 1, -fmin(200, real_precision/10.0), MPFR_RNDN);

#ifdef _DEBUG
    mpfr_printf("init w   = %.30RNf\n", w);
#endif

  bool found = false;
  for (int i = 0; i < maxiter && !found; i++) {
    mpfr_exp(e, w, MPFR_RNDN);
  mpfr_printf(" %d. w   = %.30RNf\n", i, w);
  mpfr_printf(" %d. e   = %.30RNf\n", i, e);
    mpfr_add_si(p, w, 1, MPFR_RNDN);
  mpfr_printf(" %d. p   = %.30RNf\n", i, p);
    mpfr_mul(tmp, w, e, MPFR_RNDN);
  mpfr_printf(" %d. tmp = %.30RNf\n", i, tmp);
    mpfr_sub(t, tmp, x, MPFR_RNDN);
  mpfr_printf(" %d. t   = %.30RNf\n", i, t);

    if (mpfr_sgn(w) > 0) {
      mpfr_div(tmp, t, p, MPFR_RNDN);
      mpfr_div(t, tmp, e, MPFR_RNDN);
    } else {
      mpfr_mul(tmp2, e, p, MPFR_RNDN);
    mpfr_printf(" %d. e   = %.30RNf\n", i, e);
    mpfr_printf(" %d. p   = %.30RNf\n", i, p);
    mpfr_printf(" %d. tmp2= %.30RNf\n", i, tmp2);
      mpfr_add_si(tmp, p, 1, MPFR_RNDN);
    mpfr_printf(" %d. tmp = %.30RNf\n", i, tmp);
      mpfr_mul_d(tmp, tmp, 0.5, MPFR_RNDN);
    mpfr_printf(" %d. tmp = %.30RNf\n", i, tmp);
      mpfr_mul(tmp, tmp, t, MPFR_RNDN);
    mpfr_printf(" %d. tmp = %.30RNf\n", i, tmp);
      mpfr_div(tmp, tmp, p, MPFR_RNDN);
    mpfr_printf(" %d. tmp = %.30RNf\n", i, tmp);
      mpfr_sub(tmp, tmp2, tmp, MPFR_RNDN);
    mpfr_printf(" %d. tmp = %.30RNf\n", i, tmp);
      mpfr_div(t, t, tmp, MPFR_RNDN);
    mpfr_printf(" %d. t   = %.30RNf\n", i, t);
    }
    mpfr_sub(w, w, t, MPFR_RNDN);

#ifdef _DEBUG
    mpfr_printf(" %d. w   = %.30RNf\n", i, w);
#endif
    
    mpfr_abs(tmp, p, MPFR_RNDN);
  mpfr_printf(" %d. tmp = %.30RNf\n", i, tmp);
    mpfr_mul(tmp, tmp, e, MPFR_RNDN);
  mpfr_printf(" %d. tmp = %.30RNf\n", i, tmp);
    mpfr_d_div(tmp, 1, tmp, MPFR_RNDN);
  mpfr_printf(" %d. tmp = %.30RNf\n", i, tmp);
    mpfr_abs(tmp2, w, MPFR_RNDN);
  mpfr_printf(" %d. tmp2= %.30RNf\n", i, tmp2);
    mpfr_max(tmp, tmp, tmp2, MPFR_RNDN);
  mpfr_printf(" %d. tmp = %.30RNf\n", i, tmp);
    mpfr_mul_si(tmp, tmp, 10, MPFR_RNDN);
  mpfr_printf(" %d. tmp = %.30RNf\n", i, tmp);
    mpfr_mul(tol, eps, tmp, MPFR_RNDN);
  mpfr_printf(" %d. eps = %.30RNf\n", i, eps);
  mpfr_printf(" %d. tol = %.30RNf\n", i, tol);
    
    mpfr_abs(tmp, t, MPFR_RNDN);
    if (mpfr_cmp(tol, tmp) > 0) {
      found = true;
    }
  }

#ifdef _DEBUG
  if (!found) {
    std::cout << "WARNING: Lambert maxiter!" << std::endl;
  }
  mpfr_printf("  w = %.30RNf\n", w);
  mpfr_printf("  t = %.30RNf\n", t);
#endif

  mpfr_neg(w, w, MPFR_RNDN);
  mpfr_exp(w, w, MPFR_RNDN);
  mpf_class ww(0, real_precision);
  mpfr_get_f(ww.get_mpf_t(), w, MPFR_RNDN);
  mpz_class result(ceil(ww));

  mpfr_free_cache();
  mpfr_clears(L1, L2, w, x, eps, tol, e, p, t, tmp, tmp2, NULL);

  return result;
}

int main() {
  const int lambda = 51;
  //std::cout << _expmLambertWm1(lambda) << endl << endl;
  const mpz_class x = expmLambertWm1(lambda);
  std::cout << "  x = " << x << endl;

  mpfr_t z, e2l, tmp;
  mpfr_inits2(real_precision, z, e2l, tmp, NULL);
  mpfr_set_f(z, mpf_class(x).get_mpf_t(), MPFR_RNDN);
  mpfr_log(tmp, z, MPFR_RNDN);
  mpfr_div(z, z, tmp, MPFR_RNDN);
  mpfr_set_si_2exp(e2l, 1, lambda, MPFR_RNDN);

  mpfr_printf("\n\n  z = %.30RNf\n", z);
  mpfr_printf(    "e2l = %.30RNf\n", e2l);

  mpfr_free_cache();
  mpfr_clears(z, e2l, tmp, NULL);

  return 0;
}
