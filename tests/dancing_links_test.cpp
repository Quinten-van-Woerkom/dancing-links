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
  item item{};
  option owner{42};
  owner.add_item(item);

  for (auto &node : item.covering_options()) {
    REQUIRE(node.parent_option().get_index() == 42);
  }

  REQUIRE(item.count() == 1);
}

TEST_CASE("A node can remove itself reversibly from a linked list", "[node]") {
  item dummy_owner{};
  option dummy_option{2};
  /*std::vector<node> nodes{{nullptr, nullptr, dummy_owner, dummy_option},
                          {nullptr, nullptr, dummy_owner, dummy_option}};

  list_view<node> list{nodes};*/
  linked_list<node> list;
  list.emplace_back(nullptr, nullptr, dummy_owner, dummy_option);
  list.emplace_back(nullptr, nullptr, dummy_owner, dummy_option);

  REQUIRE(list.size() == 2);

  list[0].remove();
  REQUIRE(list.size() == 1);

  list[1].remove();
  REQUIRE(list.size() == 0);

  list[0].reinsert();
  list[1].reinsert();
  REQUIRE(list.size() == 2);
}