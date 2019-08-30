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
/// Data structures for Knuth's dancing links algorithm solving the exact cover
/// problem.
///
//===----------------------------------------------------------------------===//

#pragma once

#include <initializer_list>
#include <memory>
#include <vector>

namespace sudo {
/// A dancing links matrix is a boolean matrix, stored as a circular four-way
/// doubly linked lists of nodes representing the ones in each row and column.
class column_header;
class node;

/// Each node contains pointers to its four neighbours and to its column owner.
class node {
public:
  /// A normal node must be fully initialized, no nullptr allowed.
  node(node* left, node *right, node *up, node *down, column_header* owner, std::size_t row_index)
      : left {left}, right {right}, up {up}, down {down}, owner {owner}, index {row_index} {}

  /// Removes the node's row from the column list.
  void cover();

  /// Backtracks and readds a row.
  void uncover();

  /// Links nodes
  /// @{
  void link_left(node *node) { this->left = node; }
  void link_right(node *node) { this->right = node; }
  void link_up(node *node) { this->up = node; }
  void link_down(node *node) { this->down = node; }
  /// @}

  /// The node can remove itself from a linked list, linking up its neighbours.
  /// @{
  void remove_from_row();
  void remove_from_column();
  /// @}

  /// The node can reinsert itself back into a linked list, linking its
  /// neighbours to itself.
  /// @{
  void reinsert_into_row();
  void reinsert_into_column();
  /// @}
  
private:
  /// Neighbouring nodes
  node *left = nullptr, *right = nullptr, *up = nullptr, *down = nullptr;

  /// Owning column object
  column_header *owner = nullptr;

  /// Row index of the node, column size if node is a column header.
  std::size_t index = 0;

  /// Required to allow direct access for column to the neighbouring nodes.
  friend class column_header;
  friend class matrix;
};

/// Column header
/// Column owner; special case is the root, owning all columns, which only
/// contains left and right pointers, the rest being null.
class column_header : public node {
public:
  

  /// Removes this column from the header list, as well as any rows containing
  /// a one in this column.
  void cover();

  /// Backtracks and readds a column with the corresponding removed rows.
  void uncover();

  /// Inserts a node with given neighbours into the column.
  void insert_node(node *left, node *right, std::size_t row_index);

private:
  char name = 0;
  std::vector<node> nodes = {};

  /// Required to allow manipulation of size from inside nodes.
  friend class node;
  friend class matrix;
};

/// Owns all columns and acts as root node.
class matrix : public column_header {
public:
  matrix(std::initializer_list<std::initializer_list<std::size_t>> subsets);

  /// Search the constraint space for solutions
  void search();

private:
  std::vector<std::vector<node *>> result;
  std::vector<node*> cache;
  std::vector<column_header> columns;
};
}