//===-- main.cpp - Dancing links --------------------------------*- C++ -*-===//
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
/// Entrance for the dancing links application. Solves a given exact cover
/// problem.
///
//===----------------------------------------------------------------------===//

#include <initializer_list>
#include <iostream>

#include "dancing_links.h"

using namespace dlx;

#include <array>

int main() {
  auto problem = dancing_links(4, {{1, 2}, {0}, {0, 3}, {3}});

  problem.solve_fully();
  auto solutions = problem.solutions();
  for (auto solution : solutions) {
    for (auto option : solution) {
      std::cout << option->get_index() << ", ";
    }
    std::cout << '\n';
  }

  std::cout << "Hello, world!\n";
}