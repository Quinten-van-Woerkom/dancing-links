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

#include <memory>
#include <vector>

namespace sudo {
/// A dancing links matrix is a boolean matrix, stored as a circular four-way
/// doubly linked lists of nodes representing the ones in each row and column.
class column;
class node;

/// Each node contains pointers to its four neighbours and to its column owner.
class node {
public:
  /// Removes the node's row from the column list.
  void cover();

  /// Backtracks and readds a row.
  void uncover();
  
private:
  /// Neighbouring nodes
  node *left, *right, *up, *down;

  /// Owning column object
  column *owner;

  /// Required to allow direct access for column to the neighbouring nodes.
  friend class column;
  friend class matrix;
};

/// Column owner
/// Can be the column owner; special case which only contains left and right
/// pointers, the rest being null.
class column : public node {
public:
  /// Removes this column from the header list, as well as any rows containing
  /// a one in this column.
  void cover();

  /// Backtracks and readds a column with the corresponding removed rows.
  void uncover();

private:
  std::size_t size;
  std::vector<node> nodes;

  /// Required to allow manipulation of size from inside nodes.
  friend class node;
  friend class matrix;
};

/// Owns all columns and acts as root node.
class matrix : public column {
public:
  void search();

private:
  std::vector<node*> result;
  std::vector<column> columns;
};
}