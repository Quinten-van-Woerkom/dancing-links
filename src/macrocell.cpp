//===-- macrocell.cpp - Macrocell class definition --------------*- C++ -*-===//
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
/// Implementation of the macrocell type representing squares of cells of
/// arbitrary size.
///
//===----------------------------------------------------------------------===//

#include "macrocell.h"

#include "rules.h"

namespace life {

//===-- Leaf --------------------------------------------------------------===//
auto leaf::create(square<4> nw, square<4> ne, square<4> sw, square<4> se)
    -> leaf {
  return leaves.emplace(nw, ne, sw, se);
}

leaf::leaf(square<4> nw, square<4> ne, square<4> sw, square<4> se)
    : cells{nw, ne, sw, se} {
  cached_next = cells.next();
  cached_future = cached_next.next();
}

leaf::leaf(square<8> cells) : cells{cells} {
  cached_next = cells.next();
  cached_future = cached_next.next();
}
} // namespace life