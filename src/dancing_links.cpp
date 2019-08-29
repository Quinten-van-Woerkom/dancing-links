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

#include <limits>

using namespace sudo;

void node::cover() { column->cover(); }

void node::uncover() { column->cover(); }

void column::cover() {
  this->right->left = this->left;
  this->left->right = this->right;

  for (auto i = this->down; i != this; i = i->down) {
    for (auto j = this->right; j != i; j = j->right) {
      j->down->up = j->up;
      j->up->down = j->down;
      j->owner->size -= 1;
    }
  }
}

void column::uncover() {
  for (auto i = this->up; i != this; i = i->up) {
    for (auto j = this->left; j != i; j = j->left) {
      j->owner->size += 1;
      j->down->up = j;
      j->up->down = j;
    }
  }

  this->right->left = this;
  this->left->right = this;
}

void matrix::search() {
  if (this->right == this)
    return;

  /// We choose the column object c to operate on based on minimum size.
  auto s = std::numeric_limits<std::size_t>::max();
  auto c = static_cast<column*>(nullptr);
  for (auto j = this->right; j != this; j = j->right) {
    if (j->owner->size < s) {
      c = j->owner;
      s = j->owner->size;
    }
  }

  c->cover();
  for (auto r = c->down; r != c; r = r->down) {
    result.push_back(r);
    for (auto j = r->right; j != r; j = j->right) {
      j->cover();
    }

    // We need to go deeper
    search();

    r = result.pop_back();
    c = r->column;
    for (auto j = r->left; j != r; j = j->left) {
      j->uncover();
    }
  }
  c->uncover();
}
