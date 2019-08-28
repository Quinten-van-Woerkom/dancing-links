#include <iostream>

#include "bitmap.h"
#include "macrocell.h"

using namespace life;

int main() {
  hashtable<leaf> leaves;
  leaves.emplace(1, 0, 0, 0);
  leaves.emplace(0, 0, 0, 0);
  leaves.emplace(0, 0, 0, 0);
  leaves.emplace(1, 0, 0, 0);
  std::cout << "Hello, world!\n";
}