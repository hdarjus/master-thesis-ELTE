#pragma once

#include "types.h"
#include "Lottery.hpp"

class Player {
public:
  Player(const int m);
  ~Player() = default;

  void add_money(const int m);

  template<VDF_version w>
  void play(Lottery<w>& l, const int m);
  
private:
  int money;
};

