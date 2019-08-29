//===-- sudoku.cpp - Life rules test ----------------------------*- C++ -*-===//
//
// Hashlife
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
/// Entrance for the sudoku solver application. Solves a sudoku as described in
/// a given file.
///
//===----------------------------------------------------------------------===//

#include <iostream>

#include "sudoku.h"

using namespace sudoku;

int main() {
  cell cell{};
  cell.prune(1, 2, 3, 5, 6, 7, 8, 9);
  std::cout << cell.solution() << '\n' << cell.count() << '\n';
  std::cout << "Hello, world!\n";
}