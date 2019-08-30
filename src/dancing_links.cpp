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

#include <iostream>
#include <limits>

using namespace sudo;

/// When a node is covered, it means that its owning column is covered in its
/// entirety.
void node::cover() { owner->cover(); }

/// Uncovering a node is equivalent to uncovering its column.
void node::uncover() { owner->cover(); }

/// Removal relinks the neighbours together, but leaves the removed node's
/// pointers intact, to allow for reinsertion.
void node::remove_from_row() {
  this->right->left = this->left;
  this->left->right = this->right;
}

/// Removal from a column also decreases the column size.
void node::remove_from_column() {
  this->down->up = this->up;
  this->up->down = this->down;
  this->owner->index -= 1;
}

/// Reinsertion relinks the neighbours back to the node itself.
void node::reinsert_into_row() {
  this->right->left = this;
  this->left->right = this;
}

/// Reinsertion into a column also increases the column size.
void node::reinsert_into_column() {
  this->index += 1;
  this->down->up = this;
  this->up->down = this;
}

/// When a column is covered, it is removed from the column list, and any rows
/// with a one at this column position are removed as well, since they are no
/// longer valid solutions to the exact cover problem.
void column_header::cover() {
  this->remove_from_row();

  for (auto i = this->down; i != this; i = i->down) {
    for (auto j = i->right; j != i; j = j->right) {
      j->remove_from_column();
    }
  }
}

/// When a column is uncovered, it and any removed rows again become
/// considerations in the exact cover problem.
void column_header::uncover() {
  for (auto i = this->up; i != this; i = i->up) {
    for (auto j = i->left; j != i; j = j->left) {
      j->reinsert_into_column();
    }
  }

  this->reinsert_into_row();
}

/// Inserts a node at the end of the column with the given neighbours.
void column_header::insert_node(node *left_node, node *right_node, std::size_t row_index) {
  nodes.push_back({left_node, right_node, this->up, this, this, row_index});
}

/// Constructs a dancing links matrix representation of the given subsets
/// representing the exact cover problem.
matrix::matrix(
    std::initializer_list<std::initializer_list<std::size_t>> subsets) {
  for (const auto &subset : subsets) {
    for (const auto &column : subset) {
      
    }
  }
}

/// Searches the constraint space for a solution to the formulated exact cover
/// problem.
void matrix::search() {
  if (this->right == this) {
    result.push_back(cache);
    for (auto n : cache) {
      std::cout << n->owner->name << ' ';
    }
    std::cout << '\n';
    return;
  }

  /// We choose the column object c to operate on based on minimum size.
  auto size = std::numeric_limits<std::size_t>::max();
  auto c = static_cast<column_header *>(nullptr);
  for (auto j = this->right; j != this; j = j->right) {
    if (j->owner->index < size) {
      c = j->owner;
      size = j->owner->index;
    }
  }

  c->cover();
  for (auto r = c->down; r != c; r = r->down) {
    cache.push_back(r);
    for (auto j = r->right; j != r; j = j->right) {
      j->cover();
    }

    // We need to go deeper
    search();

    r = cache.back();
    cache.pop_back();
    c = r->owner;
    for (auto j = r->left; j != r; j = j->left) {
      j->uncover();
    }
  }
  c->uncover();
}
