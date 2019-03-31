#pragma once

#include "Verifier.hpp"
#include "Player.h"
#include <functional>
#include <utility>

using PlayerNULL = std::unique_ptr<Player>;

template<VDF_version m>
class Lottery {
public:
  using solution = typename Verifier<m>::solution;

  bool submit_solution(const solution& sol);

private:
  Verifier<m> verifier;
  int pot;
  std::function<bool(const bytevec&)> judge;
  std::pair<PlayerNULL, PlayerNULL> players;
};

