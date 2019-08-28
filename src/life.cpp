#include <iostream>

#include "macrocell.h"

using namespace life;

int main() {
  auto origin = bitmap(R"(
    00000000
    00000000
    00000000
    00000000
    00111000
    00000000
    00000000
    00000000
  )");

  auto nw = origin.nw().shift(2, 2);
  auto ne = origin.ne().shift(-2, 2);
  auto sw = origin.sw().shift(2, -2);
  auto se = origin.se().shift(-2, -2);

  std::cout << origin << '\n';
  std::cout << origin.next(4) << '\n';
}