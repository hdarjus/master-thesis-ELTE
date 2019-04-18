#include "../include/ProverWesolowski.h"

ProverWesolowski::ProverWesolowski() :
    ctx_ptr(BN_CTX_free_ptr(BN_CTX_secure_new(), ::BN_CTX_free)) { }

typename ProverWesolowski::solution
ProverWesolowski::operator()(const VerifierWesolowski& verifier) const {
  BN_CTX* ctx = ctx_ptr.get();
  BN_CTX_start(ctx);

  // constants
  const RSWPuzzle puzzle = verifier.get_RSWPuzzle();
  Hash2Prime hash = verifier.get_Hash();
  const unsigned long t = puzzle.get_log2T();
  const bytevec _T = puzzle.get_T();
  const bytevec _N = puzzle.get_N();
  const bytevec _x = puzzle.get_x();

  // helper variables
  BIGNUM* x = BN_CTX_get(ctx);
  BIGNUM* T = BN_CTX_get(ctx);
  BIGNUM* N = BN_CTX_get(ctx);
  BIGNUM* pi = BN_CTX_get(ctx);
  BIGNUM* tt = BN_CTX_get(ctx);
  BIGNUM* p = BN_CTX_get(ctx);
  BIGNUM* r = BN_CTX_get(ctx);
  BIGNUM* y = BN_CTX_get(ctx);
  BIGNUM* xy = BN_CTX_get(ctx);

  // set initial values
  BN_bin2bn(_x.data(), (int)_x.size(), x);
  BN_bin2bn(_T.data(), (int)_T.size(), T);
  BN_bin2bn(_N.data(), (int)_N.size(), N);

  // declare returned values
  bytevec _y;
  bytevec _pi;

  // compute y
  BN_copy(y, x);
  for (BN_one(tt); BN_cmp(tt, T) <= 0;  BN_add(tt, tt, BN_value_one())) {
    BN_mod_sqr(y, y, N, ctx);
  }
  bn2bytevec(y, _y);
#ifdef _DEBUG
      std::cout << "y:\t" << print_bn(y) << std::endl;
#endif

  // hash x||y
  BN_copy(xy, x);
  BN_lshift(xy, xy, BN_num_bits(N));
  BN_add(xy, xy, y);
  hash(xy, p);
#ifdef _DEBUG
      //BN_set_word(p, 7);
      std::cout << "p:\t" << print_bn(p) << std::endl;
#endif

  // compute pi
  BN_one(r);
#ifdef _DEBUG
      std::cout << "r:\t" << print_bn(r) << std::endl;
#endif
  BN_one(pi);
#ifdef _DEBUG
      std::cout << "pi:\t" << print_bn(pi) << std::endl;
#endif
  for (BN_one(tt); BN_cmp(tt, T) <= 0;  BN_add(tt, tt, BN_value_one())) {
#ifdef _DEBUG
      std::cout << "tt:\t" << print_bn(tt) << std::endl;
#endif
    BN_mod_sqr(pi, pi, N, ctx);
#ifdef _DEBUG
      std::cout << "pi:\t" << print_bn(pi) << std::endl;
#endif
    BN_lshift1(r, r);
#ifdef _DEBUG
      std::cout << "r:\t" << print_bn(r) << std::endl;
#endif
    if (BN_cmp(r, p) >= 0) {
      BN_sub(r, r, p);
      BN_mod_mul(pi, pi, x, N, ctx);
#ifdef _DEBUG
      std::cout << "r:\t" << print_bn(r) << std::endl;
      std::cout << "pi:\t" << print_bn(pi) << std::endl;
#endif
    }
  }
  bn2bytevec(pi, _pi);
#ifdef _DEBUG
      std::cout << "---------------------------------------------------" << std::endl;
#endif

  BN_CTX_end(ctx);
  return make_pair(_pi, _y);
}

