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

TEST_CASE("Can initialize linked list with vector", "[linked_list]") {
  std::vector<node> nodes{12};
  list_view<node> list{nodes};
  const auto og_root = &*list.end();

  auto counter = 0;
  const auto *previous = &*list.end();
  for (const auto &node : list) {
    REQUIRE(&previous->next() == &node);
    previous = &node;
    ++counter;
  }
  REQUIRE(counter == nodes.size());

  for (auto i = 1; i < nodes.size() - 1; ++i) {
    REQUIRE(&nodes[i].previous() == &nodes[i - 1]);
    REQUIRE(&nodes[i].next() == &nodes[i + 1]);
  }

  REQUIRE(&nodes.back().next() == &*list.end());
  REQUIRE(&(*list.end()).next() == &nodes.front());
  REQUIRE(&*list.end() == og_root);
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
  std::vector<node> nodes{2};
  item dummy_owner{};

  for (auto &element : nodes)
    element = node{nullptr, nullptr, &dummy_owner, nullptr};

  list_view<node> list{nodes};
  REQUIRE(list.size() == 2);

  nodes.front().remove();
  REQUIRE(list.size() == 1);

  nodes.back().remove();
  REQUIRE(list.size() == 0);

  nodes.front().reinsert();
  nodes.back().reinsert();
  REQUIRE(list.size() == 2);
}