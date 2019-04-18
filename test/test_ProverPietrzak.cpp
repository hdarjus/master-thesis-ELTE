#ifndef _DEBUG
#define _DEBUG
#endif

#include "../include/ProverPietrzak.h"
#include <iostream>
#include <sstream>
#include "../include/util.h"

int main(int argc, char* argv[]) {
  BIGNUM* N_bn = BN_secure_new();
  BN_dec2bn(&N_bn, "799979478482341");
  bytevec N = bn2bytevec(N_bn);


  /*  TEST 1  */
  VerifierPietrzak verifier(20, 8, {97}, N);
  ProverPietrzak prover;
  const auto sol = prover(verifier, 1);

  //std::vector<bytevec> pi = sol.first;
  //std::cout << print_bytevec(sol.second) << std::endl << std::endl;
  //for (int i = 0; i < pi.size(); i++) {
  //  std::cout << print_bytevec(pi[i]) << std::endl;
  //}

  std::cout << "--------------" << std::endl;

  if (verifier(sol)) {
    std::cout << "1. Good: Verified ****************************************************************************" << std::endl << std::endl << std::endl;
  } else {
    std::cout << "1. Oh no: Falsified ****************************************************************************" << std::endl << std::endl << std::endl;
  }

  //pi = sol.first;  // sol does not change
  //std::cout << print_bytevec(sol.second) << std::endl << std::endl;
  //for (int i = 0; i < pi.size(); i++) {
  //  std::cout << print_bytevec(pi[i]) << std::endl;
  //}

  std::cout << "--------------" << std::endl;

  if (verifier(sol)) {
    std::cout << "2. Good: Verified ****************************************************************************" << std::endl << std::endl << std::endl;
  } else {
    std::cout << "2. Oh no: Falsified ****************************************************************************" << std::endl << std::endl << std::endl;
  } /* END TEST 1 */


  /*  TEST 2  reproducibility */
  VerifierPietrzak verifier2(20, 8, {97}, N);
  ProverPietrzak prover2;
  const auto sol2 = prover2(verifier2, 1);
  auto sol21 = sol2;

  if (verifier2(sol2)) {
    std::cout << "3. Good: Verified ****************************************************************************" << std::endl;
  } else {
    std::cout << "3. Oh no: Falsified ****************************************************************************" << std::endl;
  }

  sol21.first[2][1]++;  // ruin the proof

  if (verifier2(sol21)) {
    std::cout << "4. Oh no: Verified" << std::endl;
  } else {
    std::cout << "4. Good: Falsified" << std::endl;
  } /* END TEST 2 */

  /*  TEST 3  different d_max */
  VerifierPietrzak verifier3(20, 8, {77, 39, 11}, N);
  ProverPietrzak prover3;
  const auto sol3 = prover3(verifier3, 3);
  auto sol31 = sol3;

  if (verifier3(sol3)) {
    std::cout << "5. Good: Verified ****************************************************************************" << std::endl;
  } else {
    std::cout << "5. Oh no: Falsified ****************************************************************************" << std::endl;
  }

  sol31.first[0][0]++;  // ruin the proof

  if (verifier3(sol31)) {
    std::cout << "6. Oh no: Verified" << std::endl;
  } else {
    std::cout << "6. Good: Falsified" << std::endl;
  } /* END TEST 3 */

  /*  TEST 4  with lambda_RSA */
  VerifierPietrzak verifier4(20, 8, {97}, 50);
  ProverPietrzak prover4;
  const auto sol4 = prover4(verifier4, 3);
  auto sol41 = sol4;

  if (verifier4(sol4)) {
    std::cout << "7. Good: Verified ****************************************************************************" << std::endl;
  } else {
    std::cout << "7. Oh no: Falsified ****************************************************************************" << std::endl;
  }

  sol41.first[0][0]++;  // ruin the proof

  if (verifier4(sol41)) {
    std::cout << "8. Oh no: Verified" << std::endl;
  } else {
    std::cout << "8. Good: Falsified" << std::endl;
  } /* END TEST 4 */

  /*  TEST 5  large values */
  VerifierPietrzak verifier5(100, 5, {97, 111, 201, 1, 0, 0, 111, 240}, 2048);
  ProverPietrzak prover5;
  const auto sol5 = prover5(verifier5, 3);
  auto sol51 = sol5;

  if (verifier5(sol5)) {
    std::cout << "7. Good: Verified ****************************************************************************" << std::endl;
  } else {
    std::cout << "7. Oh no: Falsified ****************************************************************************" << std::endl;
  }

  sol51.first[3][1]++;  // ruin the proof

  if (verifier5(sol51)) {
    std::cout << "8. Oh no: Verified" << std::endl;
  } else {
    std::cout << "8. Good: Falsified" << std::endl;
  } /* END TEST 4 */

  return 0;
}

