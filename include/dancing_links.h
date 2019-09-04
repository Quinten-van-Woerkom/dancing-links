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
/// Data structures for Donald Knuth's dancing links algorithm solving the
/// exact cover problem.
///
//===----------------------------------------------------------------------===//

#pragma once

#include <cassert>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <type_traits>
#include <vector>

#include "linked_list.h"

namespace dlx {
/// A dancing links matrix is a boolean matrix, stored as a circular four-way
/// doubly linked lists of nodes representing the ones in each row and column.
class node;
class item;
class option;

//===-- node --------------------------------------------------------------===//
/// A node can be one of two things:
/// 1. Part of an item header.
/// 2. An object denoting the presence of an item in an option.
class node {
public:
  /// Constructors for an item header and item.
  /// @{
  node();
  node(node *up, node *down, item *top, option *owner);
  /// @}

  /// A node can cover or uncover its parent item.
  /// @{
  void cover();
  void uncover();
  /// @}

  /// A node can hide or unhide its parent option.
  /// @{
  void hide();
  void unhide();
  /// @}

  /// A node can remove and reinsert itself from and into its linked list.
  /// @{
  void remove();
  void reinsert();
  /// @}

  /// Traversal of all options covering an item.
  /// @{
  auto next() const -> const node & { return *down; };
  auto next() -> node & { return *down; };
  auto previous() const -> const node & { return *up; };
  auto previous() -> node & { return *up; };
  /// @}

  /// Links another node to be this node's neighbour.
  /// @{
  void link_previous(node &other);
  void link_next(node &other);
  /// @}

  /// Returns the option of which this node is part.
  // auto parent_option() const noexcept -> const option & { return *owner; };
  auto parent_option() noexcept -> option & { return *owner; };

private:
  node *up, *down;
  item *top;
  option *owner;
};

//===-- option ------------------------------------------------------------===//
/// Stores the nodes referencing items covered by an option.
class option {
public:
  /// Constructor, id must be provided.
  option(std::size_t index) : index{index} {};

  /// Constructor creating an option covering the columns indexed using the
  /// given initializer list set.
  option(std::size_t index, linked_list<item> &items,
         std::initializer_list<std::size_t> set);

  /// Hides/unhides this option from the candidate solution set.
  /// @{
  void hide();
  void unhide();
  /// @}

  /// (Un)covering an option (un)covers all items part of this option.
  /// @{
  void cover();
  void uncover();
  /// @}

  /// Adds a node covering an item.
  void add_item(item &item);

  /// Returns the index corresponding with the option.
  auto get_index() const -> std::size_t { return index; };

  /// Number of items covered by this option.
  auto size() const -> std::size_t { return covered.size(); };

private:
  std::vector<node> covered = {};
  std::size_t index;
};

//===-- item --------------------------------------------------------------===//
/// Header for a list of options covering the same item.
class item {
public:
  item(item *left, item *right)
      : options{}, size{0}, left{left}, right{right} {};
  item() : options{}, size{0}, left{this}, right{this} {};

  /// An item can be covered and uncovered reversibly,
  /// signalling that it is (un)covered by the current candidate solution set.
  /// @{
  void cover();
  void uncover();
  /// @}

  /// An item can reversibly remove itself from the parent linked list.
  /// @{
  void remove() const;
  void reinsert();
  /// @}

  /// Links two items
  /// @{
  void link_previous(item &other);
  void link_next(item &other);
  /// @}

  /// An item is satisfiable in the current candidate solution if the number
  /// of options covering it is non-zero.
  auto satisfiable() const -> bool { return size; }

  /// Adds a node to the item list.
  void add_node(node &node);

  /// Determines whether or not the linked list of items that are yet to be
  /// covered is empty.
  auto empty() const noexcept -> bool { return this->right == this; };

  /// Number of possible options covering this item.
  auto count() const noexcept -> std::size_t { return size; };

  /// Returns the next item in the list.
  /// @{
  auto next() const noexcept -> const item & { return *right; };
  auto next() noexcept -> item & { return *right; };
  auto previous() const noexcept -> const item & { return *left; };
  auto previous() noexcept -> item & { return *left; };
  /// @}

  /// Iterable over all options covering this item.
  auto covering_options() noexcept -> list_view<node> & { return options; }

  /// Accessors to shrink or grow the size of the linked list of options.
  /// @{
  void shrink() { size -= 1; };
  void grow() { size += 1; };
  /// @}

private:
  item *left, *right;
  list_view<node> options;
  std::size_t size;
};

//===-- exact cover -------------------------------------------------------===//
/// Solver for the exact cover problem based on Donald Knuth's dancing links
/// algorithm. Requires the number of items, as well as the set of options to
/// search for covering subsets.
class dancing_links {
public:
  /// Constructs an exact cover problem with a given number of items.
  dancing_links(
      std::size_t n_items,
      std::initializer_list<std::initializer_list<std::size_t>> options);

  /// Searches the set of options for all subsets exactly covering all items.
  void solve_fully();

  /// Searches the set of options for a subset exactly covering all items.
  auto solve() -> std::vector<option *>;

  /// Returns the solutions, if any, to the given problem.
  auto solutions() -> std::vector<std::vector<option *>> {
    return solutions_found;
  }

private:
  /// Returns true if the current subset of options covers all items.
  auto exact_cover() const -> bool;

  /// Returns the next item to be covered.
  auto next_candidate() -> item &;

  /// Adds a new item that must be covered.
  void add_item();

  linked_list<item> items = {};
  std::vector<option> options = {};
  std::vector<option *> current_subset = {};
  std::vector<std::vector<option *>> solutions_found = {};
};
} // namespace sudo