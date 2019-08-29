//===-- bitmap_test.h - Life rules test -------------------------*- C++ -*-===//
//
// Hashlife
// Copyright(C) 2019 Quinten van Woerkom
//
// This program is free software; you can redistribute it and / or
// modify it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 51
// Franklin Street, Fifth Floor, Boston, MA 02110 - 1301 USA.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests that the life rules are correctly implemented for bitmaps by checking
/// known patterns.
///
//===----------------------------------------------------------------------===//

#include "macrocell.h"
#include "rules.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using namespace life;

constexpr auto empty = square{R"(
		00000000
		00000000
		00000000
		00000000
		00000000
		00000000
		00000000
		00000000
	)"};

constexpr auto blinker = square{R"(
		00000000
		00000000
		00000000
		00111000
		00000000
		00000000
		00000000
		00000000
	)"};

constexpr auto glider = square{R"(
		00000000
		00000000
		00001000
		00000100
		00011100
		00000000
		00000000
		00000000
	)"};

constexpr auto toad = square{R"(
		00000000
		00000000
		00000000
		00011100
		00111000
		00000000
		00000000
		00000000
	)"};

constexpr auto filled = square{R"(
		11111111
		11111111
		11111111
		11111111
		11111111
		11111111
		11111111
		11111111
	)"};

constexpr auto checkers = square{R"(
		00000000
		00101010
		01010100
		00101010
		01010100
		00101010
		01010100
		00000000
	)"};


TEST_CASE("An empty grid should remain empty across generations", "[empty]") {
  REQUIRE(empty.next(10) == empty);
}

TEST_CASE("Oscillators should be equal a period apart, but unequal otherwise",
          "[oscillator]") {
  REQUIRE(toad.next(2) == toad);
  REQUIRE(toad.next() != toad);
  REQUIRE(toad.next() == toad.next(3));

  REQUIRE(blinker.next(2) == blinker);
  REQUIRE(blinker.next() != blinker);
  REQUIRE(blinker.next() == blinker.next(3));
}

TEST_CASE("A glider shifted once diagonally equals itself 4 generations advanced",
          "[glider]") {
  REQUIRE(glider.next(4) == glider.shift(1, 1));
}