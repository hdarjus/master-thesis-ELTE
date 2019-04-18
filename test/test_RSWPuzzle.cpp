#ifndef _DEBUG
#define _DEBUG
#endif

#include "../include/RSWPuzzle.h"
#include <random>
#include <stdexcept>
#include <iostream>
#include <openssl/bn.h>

using namespace std;

int main(int argc, char* argv[]) {
  mt19937 engine(42);
  uniform_int_distribution<unsigned short> gen_byte(0, 255);

  cout << "OpenSSL Version: " << OPENSSL_VERSION_TEXT << endl;

  bytevec x(7);
  for (byte& xi : x) {
    xi = static_cast<byte>(gen_byte(engine));
  }
  cout << "x: ";
  for (const auto& i : x)
    cout << (int)i << ' ';
  cout << endl;

  bytevec N(70);
  for (byte& Ni : N) {
    Ni = static_cast<byte>(gen_byte(engine));
  }
  cout << "N: ";
  for (const auto& i : N)
    cout << (int)i << ' ';
  cout << endl;

  const unsigned long t = 101;
  const unsigned long lambda = 6;
  const unsigned long lambdaRSW = 14;

  RSWPuzzle rsw1(lambda, t, x, N);
  RSWPuzzle rsw2(lambda, t, x, lambdaRSW);
  cout << "N2: ";
  for (const auto& i : rsw2.get_N())
    cout << (int)i << ' ';
  cout << endl;

  if (rsw1.get_lambda() != lambda)
    throw runtime_error("lambda error");
  if (rsw1.get_log2T() != t)
    throw runtime_error("T error");
  if (rsw1.get_x() != x)
    throw runtime_error("x error");
  if (rsw1.get_N() != N)
    throw runtime_error("N error");

  if (rsw2.get_lambda() != lambda)
    throw runtime_error("lambda error");
  if (rsw2.get_log2T() != t)
    throw runtime_error("T error");
  //if (rsw2.get_x() != x)
  //  throw runtime_error("x error");
  if (rsw2.get_N().size()*CHAR_BIT < lambdaRSW)
    throw runtime_error("lambdaRSW error");
  return 0;
}

