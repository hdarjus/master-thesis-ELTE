#include "../include/VerifierPietrzak.h"

VerifierPietrzak::VerifierPietrzak(
    const unsigned long _lambda,
    const unsigned long _t,
    const bytevec& _x,
    const bytevec& N,
    const unsigned int _key_size,
    const unsigned int _block_size) :
      hash(_lambda, _key_size, _block_size),
      durations(4),
      puzzle(_lambda, _t, _x, N),
      ctx_ptr(BN_CTX_free_ptr(BN_CTX_secure_new(), ::BN_CTX_free)) { }

VerifierPietrzak::VerifierPietrzak(
    const unsigned long _lambda,
    const unsigned long _t,
    const bytevec& _x,
    const unsigned long _lambdaRSW,
    const unsigned int _key_size,
    const unsigned int _block_size) :
      hash(_lambda, _key_size, _block_size),
      durations(4),
      puzzle(_lambda, _t, _x, _lambdaRSW),
      ctx_ptr(BN_CTX_free_ptr(BN_CTX_secure_new(), ::BN_CTX_free)) { }

Hash VerifierPietrzak::get_Hash () const {
  return hash;
}

RSWPuzzle VerifierPietrzak::get_RSWPuzzle () const {
  return puzzle;
}

bool VerifierPietrzak::operator()(const solution& sol) const {
  durations.assign(durations.size(), decltype(durations)::value_type::zero());

  BN_CTX* ctx = ctx_ptr.get();
  BN_CTX_start(ctx);

  // constants
  const unsigned long t = puzzle.get_log2T();
  const bytevec _T = puzzle.get_T();
  const bytevec _N = puzzle.get_N();
  const bytevec _x = puzzle.get_x();
  const std::vector<bytevec> _pi = sol.first;
  const bytevec _y = sol.second;

  // helper variables
  const auto start_allocation = std::chrono::high_resolution_clock::now();
  BIGNUM* mu = BN_CTX_get(ctx);
  BIGNUM* N = BN_CTX_get(ctx);
  BIGNUM* r = BN_CTX_get(ctx);
  BIGNUM* x = BN_CTX_get(ctx);
  BIGNUM* y = BN_CTX_get(ctx);
  BIGNUM* xy_help = BN_CTX_get(ctx);
  BIGNUM* xymu = BN_CTX_get(ctx);
  BIGNUM* prod_help = BN_CTX_get(ctx);
  BIGNUM* zero = BN_CTX_get(ctx);
  durations[0] = std::chrono::high_resolution_clock::now() - start_allocation;

  // set initial values
  BN_bin2bn(_x.data(), (int)_x.size(), x);
  BN_bin2bn(_N.data(), (int)_N.size(), N);
  BN_bin2bn(_y.data(), (int)_y.size(), y);
  BN_zero(zero);

#ifdef _DEBUG
    std::cout << "N:\t" << print_bn(N) << std::endl;
#endif

  // calculate xy_help that helps the hashing step
  auto start_mu_minus_hash = std::chrono::high_resolution_clock::now();
  BN_copy(xy_help, x);
  BN_lshift(xy_help, xy_help, BN_num_bits(N));
  BN_add(xy_help, xy_help, y);
  BN_lshift(xy_help, xy_help, BN_num_bits(N));
  durations[2] = std::chrono::high_resolution_clock::now() - start_mu_minus_hash;

  // validation
  decltype(std::chrono::high_resolution_clock::now()) start_hash;
  for (int i = 1; i <= _pi.size(); i++) {
#ifdef _DEBUG
      std::cout << i << std::endl;
#endif
    start_mu_minus_hash = std::chrono::high_resolution_clock::now();
    BN_bin2bn(_pi[i-1].data(), (int)_pi[i-1].size(), mu);
    if (BN_cmp(mu, N) >= 0 || BN_cmp(mu, zero) != 1) {  // 0 < mu < N?
      BN_CTX_end(ctx);
      return false;
    }
#ifdef _DEBUG
      std::cout << "mu:\t" << print_bn_hex(mu) << std::endl;
#endif
    BN_copy(xymu, xy_help);
    BN_add(xymu, xymu, mu);
#ifdef _DEBUG
      std::cout << "xymu:\t" << print_bn_hex(xymu) << std::endl;
#endif
    durations[2] += std::chrono::high_resolution_clock::now() - start_mu_minus_hash;
    start_hash = std::chrono::high_resolution_clock::now();
    hash(xymu, r);
    durations[3] += std::chrono::high_resolution_clock::now() - start_hash;
    start_mu_minus_hash = std::chrono::high_resolution_clock::now();
#ifdef _DEBUG
      std::cout << "r:\t" << print_bn(r) << std::endl;
#endif
    // get the new x
    BN_mod_exp(prod_help, x, r, N, ctx);
    BN_mod_mul(x, prod_help, mu, N, ctx);
#ifdef _DEBUG
      std::cout << "x:\t" << print_bn(x) << std::endl;
#endif
    // get the new y
    BN_mod_exp(prod_help, mu, r, N, ctx);
    BN_mod_mul(y, prod_help, y, N, ctx);
    durations[2] += std::chrono::high_resolution_clock::now() - start_mu_minus_hash;
#ifdef _DEBUG
      std::cout << "y:\t" << print_bn(y) << std::endl;
      std::cout << "----------------" << std::endl << std::endl;
#endif
  }

#ifdef _DEBUG
  std::cout << std::endl << "-----------------------------------------------------------------" << std::endl << std::endl;
#endif

  start_mu_minus_hash = std::chrono::high_resolution_clock::now();
  BN_mod_sqr(x, x, N, ctx);
#ifdef _DEBUG
    std::cout << "x:\t" << print_bn(x) << std::endl;
    std::cout << "y:\t" << print_bn(y) << std::endl;
#endif
  bool result = BN_cmp(y, x) == 0;
  durations[2] += std::chrono::high_resolution_clock::now() - start_mu_minus_hash;
  BN_CTX_end(ctx);
  return result;
}

