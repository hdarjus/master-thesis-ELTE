#ifndef _LOTTERY_HPP_
#define _LOTTERY_HPP_

#include "types.h"
#include "Verifier.hpp"
#include "Player.h"
#include <functional>
#include <utility>

template<VDF_version m>
class Lottery {
public:
  using solution = typename Verifier<m>::solution;

  bool submit_solution(const solution& sol);

private:
  Verifier<m> verifier;
  money pot;
  std::function<bool(const bytevec&)> judge;
  std::pair<Player, Player> players;
};

#endif  // _LOTTERY_HPP_

