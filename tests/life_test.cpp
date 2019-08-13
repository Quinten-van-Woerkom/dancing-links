/**
 *	Tests that the life rules are correctly implemented by checking certain known patterns.
 *	TODO: Move this to Catch2.
 */

#include "cell.h"

using namespace life;
using namespace life::grids;

/**
 *	Registers a function which returns true upon failure of the given assertion.
 */
#define register_test(name, assertion) bool test_##name() { return !(assertion); }

/**
 *	Executes a test and adds 1 to failures if it fails.
 */
#define test(name) failures += test_##name()

register_test(empty, empty.next(10) == empty); // An empty grid should stay empy
register_test(toad, toad.next(10) == toad); // Oscillators should be equal a period apart, but unequal otherwise
register_test(blinker, blinker.next() != blinker && blinker.next(2) == blinker && blinker.next() == blinker.next(3));	// Oscillators should be equal a period apart, but unequal otherwise
register_test(glider, glider.next(4) == glider.shift(1, 1)); // A glider shifted once diagonally equals a glider 4 generations advanced

int main() {
	auto failures = 0;
	test(empty);
	test(blinker);
	test(glider);
	return failures;
}