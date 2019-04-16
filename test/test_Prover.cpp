#ifndef _DEBUG
#define _DEBUG
#endif

#include "../include/Prover.hpp"
#include <iostream>
#include <sstream>
#include "../include/util.h"

int main(int argc, char* argv[]) {
  Verifier<VDF_version::PIETRZAK> verifier(20, 64, {97}, 50);
  Prover<VDF_version::PIETRZAK> prover;
  const auto sol = prover(verifier);

  //std::vector<bytevec> pi = sol.first;
  //std::cout << print_bytevec(sol.second) << std::endl << std::endl;
  //for (int i = 0; i < pi.size(); i++) {
  //  std::cout << print_bytevec(pi[i]) << std::endl;
  //}

  std::cout << "--------------" << std::endl;

  if (verifier(sol)) {
    std::cout << "Good: Verified" << std::endl;
  } else {
    std::cout << "Oh no: Falsified" << std::endl;
  }

  //pi = sol.first;  // sol does not change
  //std::cout << print_bytevec(sol.second) << std::endl << std::endl;
  //for (int i = 0; i < pi.size(); i++) {
  //  std::cout << print_bytevec(pi[i]) << std::endl;
  //}

  std::cout << "--------------" << std::endl;

  if (verifier(sol)) {
    std::cout << "Good: Verified" << std::endl;
  } else {
    std::cout << "Oh no: Falsified" << std::endl;
  }

  // Reproducible
  Verifier<VDF_version::PIETRZAK> verifier2(20, 64, {97}, 50);
  Prover<VDF_version::PIETRZAK> prover2;
  const auto sol2 = prover2(verifier2);
  auto sol3 = sol2;

  if (verifier2(sol2)) {
    std::cout << "Good: Verified" << std::endl;
  } else {
    std::cout << "Oh no: Falsified" << std::endl;
  }

  sol3.first[2][1]++;  // ruin the proof

  if (verifier(sol3)) {
    std::cout << "Oh no: Verified" << std::endl;
  } else {
    std::cout << "Good: Falsified" << std::endl;
  }

  return 0;
}

