#include <iostream>

#include "macrocell.h"

using namespace life;

int main() {
  auto res1 = leaf::create(0x38000000);
  auto res2 = leaf::create(0x1010100000);
  std::cout << std::hex << res1.future() << '\n' << res2.next();
  std::cout << "Hello, world!\n";
}