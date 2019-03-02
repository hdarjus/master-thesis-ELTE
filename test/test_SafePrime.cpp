#include <iostream>
#include "../include/SafePrime.h"

using namespace std;

int main () {
  SafePrime p, q;
  SafePrime::seed("oub91xiy1");
  p.generate(); q.generate();
  cout << "p = " << p.print() << endl << "q = " << q.print() << endl;
  return 0;
}

