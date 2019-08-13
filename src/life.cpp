#include <iostream>

#include "cell.h"

using namespace life;

int main() {
	auto origin = grids::glider;
	auto nw = origin.shift(4, 4);
	auto ne = origin.shift(-4, 4);
	auto sw = origin.shift(4, -4);
	auto se = origin.shift(-4, -4);

	std::cout << result(nw, ne, sw, se) << '\n';
	std::cout << origin.next(4) << '\n';
}