//===-- dancing_links_test.cpp - Life rules test ----------------*- C++ -*-===//
//
// Constraint-propagating sudoku solver.
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
/// Tests the dancing links algorithm.
///
//===----------------------------------------------------------------------===//

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "../include/dancing_links.h"

#include <algorithm>

using namespace dlx;

TEST_CASE("Catch works", "[catch]") {
  REQUIRE(true == true);
  REQUIRE(false == false);
}

TEST_CASE("A linked list containing only the root behaves as an empty list",
          "[linked_list]") {
  list_view<node> list{};
  REQUIRE(list.size() == 0);
  REQUIRE(list.begin() == list.end());
}

TEST_CASE("Can traverse an item's linked list of covering options", "[item]") {
  linked_list<item> item{1};
  option owner{42, item, {{0}}};

  REQUIRE(item.size() == 1);
}

TEST_CASE("A node can remove itself reversibly from a linked list", "[node]") {
  item dummy_owner{};
  option dummy_option{2};
  linked_list<node> list;
  list.emplace_back(dummy_owner, dummy_option);
  list.emplace_back(dummy_owner, dummy_option);

  REQUIRE(list.size() == 2);

  list[0].remove();
  REQUIRE(list.size() == 1);

  list[1].remove();
  REQUIRE(list.size() == 0);

  list[0].reinsert();
  list[1].reinsert();
  REQUIRE(list.size() == 2);
}

TEST_CASE("Dancing links solver correctly identifies solutions",
    "[dancing-links]") {
  auto problem = dancing_links(4, {{1, 2}, {0}, {0, 3}, {3}});
  auto solutions = problem.solve();

  REQUIRE(solutions.size() == 2);
  REQUIRE(std::find(solutions.begin(), solutions.end(),
                    std::vector<std::size_t>{0, 1, 3}) != solutions.end());
}

TEST_CASE("Dancing links solver correctly identifies absence of solutions",
    "[dancing-links]") {
  auto problem = dancing_links(4, {{0, 1, 2}, {2, 3}});
  auto solutions = problem.solve();

  REQUIRE(solutions.empty());
}

TEST_CASE("Dancing links solver can handle multiple identical options",
          "[dancing-links]") {
  auto problem = dancing_links(4, {{1, 2}, {0}, {0, 3}, {3}, {0}, {3}});
  auto solutions = problem.solve();

  REQUIRE(!solutions.empty());
}

TEST_CASE("Dancing links solver can handle empty option sets",
          "[dancing-links]") {
  auto problem = dancing_links(4, {});
  auto solutions = problem.solve();

  REQUIRE(solutions.empty());
}