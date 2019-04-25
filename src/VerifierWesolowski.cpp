#include "../include/VerifierWesolowski.h"

VerifierWesolowski::VerifierWesolowski(
    const unsigned long _lambda,
    const unsigned long _t,
    const bytevec& _x,
    const unsigned long _lambdaRSW) :
      hash(_lambda),
      durations(4),
      puzzle(_lambda, _t, _x, _lambdaRSW),
      lambda(_lambda),
      ctx_ptr(BN_CTX_free_ptr(BN_CTX_secure_new(), ::BN_CTX_free)) { }

VerifierWesolowski::VerifierWesolowski(
    const unsigned long _lambda,
    const unsigned long _t,
    const bytevec& _x,
    const bytevec& N) :
      hash(_lambda),
      durations(4),
      puzzle(_lambda, _t, _x, N),
      lambda(_lambda),
      ctx_ptr(BN_CTX_free_ptr(BN_CTX_secure_new(), ::BN_CTX_free)) { }


Hash2Prime VerifierWesolowski::get_Hash () const {
  return Hash2Prime(lambda);
}

RSWPuzzle VerifierWesolowski::get_RSWPuzzle () const {
  return puzzle;
}

bool VerifierWesolowski::operator()(const solution& sol) const {
  durations.assign(durations.size(), decltype(durations)::value_type::zero());

  BN_CTX* ctx = ctx_ptr.get();
  BN_CTX_start(ctx);

  // constants
  const unsigned long t = puzzle.get_log2T();
  const bytevec _T = puzzle.get_T();
  const bytevec _N = puzzle.get_N();
  const bytevec _x = puzzle.get_x();
  const bytevec _pi = sol.first;
  const bytevec _y = sol.second;

  // helper variables
  const auto start_allocation = std::chrono::high_resolution_clock::now();
  BIGNUM* pi = BN_CTX_get(ctx);
  BIGNUM* N = BN_CTX_get(ctx);
  BIGNUM* T = BN_CTX_get(ctx);
  BIGNUM* h = BN_CTX_get(ctx);
  BIGNUM* p = BN_CTX_get(ctx);
  BIGNUM* r = BN_CTX_get(ctx);
  BIGNUM* x = BN_CTX_get(ctx);
  BIGNUM* y = BN_CTX_get(ctx);
  BIGNUM* xy = BN_CTX_get(ctx);
  BIGNUM* BN_value_two = BN_CTX_get(ctx);
  durations[0] = std::chrono::high_resolution_clock::now() - start_allocation;

  // set initial values
  BN_bin2bn(_x.data(), (int)_x.size(), x);
  BN_bin2bn(_T.data(), (int)_T.size(), T);
  BN_bin2bn(_N.data(), (int)_N.size(), N);
  BN_bin2bn(_y.data(), (int)_y.size(), y);
  BN_bin2bn(_pi.data(), (int)_pi.size(), pi);
  BN_set_word(BN_value_two, 2);
#ifdef _DEBUG
      std::cout << "x:\t" << print_bn(x) << std::endl;
      std::cout << "y:\t" << print_bn(y) << std::endl;
      std::cout << "2:\t" << print_bn(BN_value_two) << std::endl;
#endif

  // hash x||y
  auto start_mu_minus_hash = std::chrono::high_resolution_clock::now();
  BN_copy(xy, x);
  BN_lshift(xy, xy, BN_num_bits(N));
  BN_add(xy, xy, y);
  durations[2] = std::chrono::high_resolution_clock::now() - start_mu_minus_hash;
  const auto start_hash = std::chrono::high_resolution_clock::now();
  hash(xy, p);
  durations[3] = std::chrono::high_resolution_clock::now() - start_hash;
  start_mu_minus_hash = std::chrono::high_resolution_clock::now();
#ifdef _DEBUG
      //BN_set_word(p, 7);
      std::cout << "p:\t" << print_bn(p) << std::endl;
#endif
  
  // compute r
  BN_mod_exp(r, BN_value_two, T, p, ctx);
#ifdef _DEBUG
      std::cout << "r:\t" << print_bn(r) << std::endl;
#endif

  // compute h and compare to y
  BN_mod_exp(x, x, r, N, ctx);
#ifdef _DEBUG
      std::cout << "x:\t" << print_bn(x) << std::endl;
#endif
  BN_mod_exp(pi, pi, p, N, ctx);
#ifdef _DEBUG
      std::cout << "pi:\t" << print_bn(pi) << std::endl;
#endif
  BN_mod_mul(h, x, pi, N, ctx);
#ifdef _DEBUG
      std::cout << "h:\t" << print_bn(h) << std::endl;
#endif

  bool result = BN_cmp(y, h) == 0;
  durations[2] += std::chrono::high_resolution_clock::now() - start_mu_minus_hash;
  BN_CTX_end(ctx);
  return result;
}

