//===-- dancing_links.h - Dancing links -------------------------*- C++ -*-===//
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
/// Implementation of the dancing links algorithm solving the exact cover
/// problem.
///
//===----------------------------------------------------------------------===//

#include "dancing_links.h"

#include <algorithm>
#include <cassert>

using namespace dlx;

//===-- node --------------------------------------------------------------===//
/// Constructor for a normal node denoting an item in an option.
node::node(item &top, option &owner)
    : up{nullptr}, down{nullptr}, top{top}, owner{owner} {
  top.add_node(*this);
};

/// Covers the item covered by this node.
void node::cover() { top.cover(); }

/// Uncovers the item covered by this node.
void node::uncover() { top.uncover(); }

/// Hides the option of which this node is part.
void node::hide() { owner.hide(); }

/// Unhides the option of which this node is part.
void node::unhide() { owner.unhide(); }

/// A node can remove itself from its linked list by rewiring its neighbours.
/// Removal is reversible because it does not reset the removed node's
/// neighbour pointers.
void node::remove() {
  this->up->down = this->down;
  this->down->up = this->up;
  top.shrink();
}

/// A node can reinsert itself into its linked list by rewiring its
/// neighbouring nodes.
void node::reinsert() {
  this->up->down = this;
  this->down->up = this;
  top.grow();
}

/// Links another node to be this node's upper neighbour.
void node::link_previous(node &other) {
  this->up = &other;
  other.down = this;
}

/// Links another node to be this node's down neighbour.
void node::link_next(node &other) {
  this->down = &other;
  other.up = this;
}

//===-- option ------------------------------------------------------------===//
/// Creates an option covering the specified items in <items>.
option::option(std::size_t index, linked_list<item> &items,
               std::initializer_list<std::size_t> set)
    : index{index} {
  covered.reserve(set.size());
  for (auto item : set) {
    covered.emplace_back(items[item], *this);
  }
}

/// Hides an option from the candidate solution set.
void option::hide() {
  for (auto &node : covered)
    node.remove();
}

/// Unhides an option from the candidate solution set.
void option::unhide() {
  for (auto &node : covered)
    node.reinsert();
}

/// Covers all items covered by this option.
void option::cover() {
  for (auto &node : covered) {
    node.cover();
  }
}

/// Uncovers all items covered by this option.
void option::uncover() {
  for (auto &node : covered)
    node.uncover();
}

//===-- item --------------------------------------------------------------===//
/// An item can be covered when an option containing it has been selected as
/// part of the candidate solution set, removing all options containing this
/// item from the solution set.
/// Covering is reversible.
void item::cover() {
  for (auto &option : options)
    option.hide();
  this->remove();
}

/// Reverts the covering of an option, adding it back into the candidate
/// solution set, effectively walking one step back up the search tree.
void item::uncover() {
  this->reinsert();
  for (auto &option : options)
    option.unhide();
}

/// An item can remove itself from its linked list by rewiring its neighbours.
/// Removal is reversible because it does not reset the removed node's
/// neighbour pointers.
void item::remove() const {
  this->left->right = this->right;
  this->right->left = this->left;
}

/// An item can reinsert itself into its linked list by rewiring its
/// neighbouring nodes.
void item::reinsert() {
  this->left->right = this;
  this->left->right = this;
}

/// Links another item to be the left neighbour of this item.
void item::link_previous(item &other) {
  this->left = &other;
  other.right = this;
}

/// Links another item to be the right neighbour of this item.
void item::link_next(item &other) {
  this->right = &other;
  other.left = this;
}

/// Adds a node to the end of the item list.
void item::add_node(node &node) {
  options.push_back(node);
  size += 1;
}

//===-- dancing links -----------------------------------------------------===//
/// Constructs an exact cover problem with a given number of items.
dancing_links::dancing_links(
    std::size_t n_items,
    std::initializer_list<std::initializer_list<std::size_t>> sets)
    : items{n_items} {
  options.reserve(sets.size());
  for (const auto set : sets) {
    options.emplace_back(options.size(), items, set);
  }
}

/// Recursively searches the set of options to find all subsets exactly
/// covering all given items. Resulting covering subsets are stored in
/// <solutions>.
auto dancing_links::solve() -> std::vector<std::vector<std::size_t>> {
  if (this->exact_cover()) {
    solutions.push_back(current_subset);
    return solutions;
  }

  auto &item = next_candidate();

  if (!item.satisfiable()) { // Current subset is invalid
    return solutions;
  }

  for (auto &node : item.covering_options()) {
    auto &option = node.parent_option();
    current_subset.push_back(option.get_index());
    option.cover();
    solve();
    option.uncover();
    current_subset.pop_back();
  }
  return solutions;
}

/// Recursively searches the set of options to find a subset exactly
/// covering all given items.
auto dancing_links::quicksolve() -> std::vector<std::size_t> {
  if (this->exact_cover()) {
    return current_subset;
  }

  auto &item = next_candidate();

  if (!item.satisfiable()) { // Current subset is invalid
    return {};
  }

  for (auto &node : item.covering_options()) {
    auto &option = node.parent_option();
    current_subset.push_back(option.get_index());
    option.cover();
    auto result = quicksolve();
    if (!result.empty())
      return result;
    option.uncover();
    current_subset.pop_back();
  }
  return {};
}

/// Returns true if the current subset of options covers all items.
/// Determines whether or not this is the case by testing if the linked list of
/// items that remain to be covered is empty.
auto dancing_links::exact_cover() const -> bool {
  return items.empty();
}

/// Returns the next item to be covered.
/// Current heuristic for determining this candidate is the one with
/// the smallest size.
auto dancing_links::next_candidate() -> item & {
  return *std::min_element(items.begin(), items.end(),
                           [](const auto &left, const auto &right) {
                             return left.count() < right.count();
                           });
}