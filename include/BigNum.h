#ifndef _BIG_NUM_H_
#define _BIG_NUM_H_

extern "C" {
#include <openssl/bn.h>
}

#include <string>

class BigNum {
public:
  BigNum ();
  BigNum (const BigNum& rhs);
  ~BigNum ();

  BigNum& operator= (const BigNum& rhs);
  const BigNum operator+ (const BigNum& rhs) const;
  const BigNum operator- (const BigNum& rhs) const;
  const BigNum operator* (const BigNum& rhs) const;
  const BigNum operator/ (const BigNum& rhs) const;
  const BigNum operator% (const BigNum& rhs) const;
  BigNum& operator+= (const BigNum& rhs);
  BigNum& operator-= (const BigNum& rhs);
  BigNum& operator*= (const BigNum& rhs);
  BigNum& operator/= (const BigNum& rhs);
  BigNum& operator%= (const BigNum& rhs);
  bool operator== (const BigNum& rhs) const;
  bool operator!= (const BigNum& rhs) const;
  void set (const int n);
  void set (const std::string s);
  std::string print () const;

private:
  BIGNUM* value;
};

#endif  // _BIG_NUM_H_

