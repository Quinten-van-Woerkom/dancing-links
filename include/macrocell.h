//===-- macrocell.h - Macrocell class definition ----------------*- C++ -*-===//
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

#pragma once

#include "rules.h"

#include <cstdint>
#include <string_view>
#include <tuple>

namespace life {
//===-- 8-square ----------------------------------------------------------===//
/// Single square bitmap, representing an 8x8 cell block.
/// This is the smallest unit of operation; individual cells are stored as bits.
/// Calculation of its next states is done directly, every time, due to the low
/// cost of evaluation.
class bitmap {
public:
  /// Direct construction from an 64-bit unsigned.
  constexpr bitmap(std::uint64_t cells = 0) noexcept : cells{cells} {}

  /// Constructs a bitmap from a string_view, interpreting any 1 as a living
  /// cell, any 0 as a dead cell, and ignoring any other characters.
  constexpr bitmap(std::string_view init) noexcept : cells{} {
    auto counter = 0;
    for (auto c : init) {
      if (c == '1')
        set(counter++, true);
      if (c == '0')
        set(counter++, false);
    }
  }

  /// Constructs the bitmap from four 4-squares, interpreted as quadrants of
  /// the new 8-square.
  constexpr bitmap(bitmap nw, bitmap ne, bitmap sw, bitmap se) noexcept
      : cells{} {
    cells = nw.shift(-2, -2);
    cells |= ne.shift(2, -2);
    cells |= sw.shift(-2, 2);
    cells |= se.shift(2, 2);
  }

  /// Accessors returning only single quarters of the bitmap, shifted to be in
  /// the center.
  /// @{
  constexpr auto nw() const noexcept -> bitmap {
    return this->shift(2, 2).subsquare(4);
  }

  constexpr auto ne() const noexcept -> bitmap {
    return this->shift(-2, 2).subsquare(4);
  }

  constexpr auto sw() const noexcept -> bitmap {
    return this->shift(2, -2).subsquare(4);
  }

  constexpr auto se() const noexcept -> bitmap {
    return this->shift(-2, -2).subsquare(4);
  }
  /// @}

  /// Returns the centred subsquare of given level (width) by setting the square
  /// boundaries to 0.
  constexpr auto subsquare(size_t level) const noexcept -> bitmap {
    if (level == 2)
      return cells & 0x0000001818000000;
    if (level == 4)
      return cells & 0x00003c3c3c3c0000;
    else
      return cells & 0x007e7e7e7e7e7e00;
  }

  /// Provides grid-based indexing into the cell.
  /// Upper left is (0, 0), lower right (7, 7).
  constexpr auto alive(std::size_t x, std::size_t y) const noexcept -> bool {
    return bit(cells, x + 8 * y);
  }

  /// Calculates the result of the bitmap, that is, its state 2 generations in
  /// advance. Any cells outside the centre 4x4 square are set to 0, their
  /// future requires information about external cells.
  constexpr auto result() const noexcept -> bitmap {
    return next(2).subsquare(4);
  }

  /// Calculates the next generation of the central 6x6 square.
  /// Edges are returned as 0: information on surrounding squares would be
  /// necessary to calculate their values.
  constexpr auto next() const noexcept -> bitmap {
    const auto left = cells << 1;
    const auto right = cells >> 1;
    const auto [mid1, mid2] = full_adder(left, cells, right);

    const auto up1 = mid1 << 8;
    const auto up2 = mid2 << 8;
    const auto down1 = mid1 >> 8;
    const auto down2 = mid2 >> 8;

    const auto [sum1, sum2a] = full_adder(up1, mid1, down1);
    const auto [sum2b, sum4a] = full_adder(up2, mid2, down2);
    const auto [sum2, sum4b] = half_adder(sum2a, sum2b);
    const auto sum4 = sum4a ^ sum4b;

    const auto result = cells & (~sum1 & ~sum2 & sum4) | (sum1 & sum2 & ~sum4);
    return bitmap{result & 0x007e7e7e7e7e7e00};
  }

  /// Calculates the state <generations> in advance.
  /// Only valid for grids resulting in empty cells along the boundaries.
  constexpr auto next(std::size_t generations) const noexcept -> bitmap {
    auto result = bitmap{*this};
    for (auto i = 0; i < generations; ++i) {
      result = result.next();
    }
    return result;
  }

  /// Returns the cell shifted down and to the right by the specified amounts.
  constexpr auto shift(int right, int down) const noexcept -> bitmap {
    auto shifted = right + 8 * down;
    if (shifted > 0)
      return bitmap{cells << shifted};
    else
      return bitmap{cells >> -shifted};
  }

  /// Sets the cell at <index> to either alive (true) or dead (false).
  constexpr void set(std::size_t index, bool alive = true) noexcept {
    cells = (cells & ~(1ull << index)) | (uint64_t(alive) << index);
  }

  /// Sets the cell at (x, y) to either alive or dead.
  constexpr void set(std::size_t x, std::size_t y, bool alive = true) noexcept {
    this->set(x + 8 * y, alive);
  }

  /// The bitmap can be converted implicitly to its 64-bit unsigned
  /// representation.
  constexpr operator const std::uint64_t &() const noexcept {
    return this->cells;
  };
  constexpr operator std::uint64_t &() noexcept { return this->cells; };

private:
  /// Cell states are stored as bits in a 64-bit unsigned integer, with the low
  /// bit representing the cell in the upper-left corner and the high bit
  /// representing the lower-right cell.
  std::uint64_t cells;
};

auto &operator<<(std::ostream &os, bitmap other) {
  for (auto y = 0; y < 8; ++y) {
    for (auto x = 0; x < 8; ++x) {
      if (other.alive(x, y))
        os << "\xdb\xdb";
      else
        os << "[]";
    }
    os << '\n';
  }
  return os;
}

//===-- Leaf --------------------------------------------------------------===//
/// Representation of a 16x16 square. End of recursion in the quadtree.
class leaf {
public:
  constexpr auto result() const noexcept -> bitmap {
    return result(nw, ne, sw, se);
  }

private:
  // A null sentinel value indicates that a macrocell is a leaf.
  void *sentinel;
  bitmap nw, ne, sw, se;
};

//===-- Node --------------------------------------------------------------===//
/// Representation of squares bigger than 16x16; simple nodes in the
/// quadtree.
class node {
public:
  constexpr auto result() const noexcept -> node {
    return result(nw, ne, sw, se);
  }

  /// When a node's children are leaves, its result must be calculated differently.
  constexpr auto result_leaf() const noexcept -> leaf {
    return result(*reinterpret_cast<leaf *>(nw), *reinterpret_cast<leaf *>(ne),
                  *reinterpret_cast<leaf *>(sw), *reinterpret_cast<leaf *>(se));
  }

private:
  /// Depending on the depth, pointers point to other nodes, or to leaves.
  void *nw, *ne, *sw, *se;
  void *result;
};


//===-- Macrocell ---------------------------------------------------------===//
/// Represents squares of any size.
class macrocell {
public:
  /// Macrocells can only be created through factory functions, to allow for
  /// canonisation.
  macrocell() = delete;

  /// Factory functions to support canonisation.
  /// @{
  static constexpr auto create(macrocell nw, macrocell ne, macrocell sw,
                               macrocell se) -> macrocell *;
  static constexpr auto create(node nw, node ne, node sw, node se)
      -> macrocell *;
  static constexpr auto create(leaf nw, leaf ne, leaf sw, leaf se)
      -> macrocell *;
  static constexpr auto create(bitmap nw, bitmap ne, bitmap sw, bitmap se)
      -> macrocell *;
  /// @}

  /// The first 64 bits of a macrocell are interpreted as a sentinel pointer
  /// denoting whether it is a node (not null) or a leaf (null).
  constexpr auto is_node() const noexcept -> bool { return this->leaf.sentinel; }
  constexpr auto is_leaf() const noexcept -> bool { return !this->leaf.sentinel; }

  /// A macrocell's depth indicates the number of macrocells below it.
  /// Counting starts at 4 to be consistens with Gosper's n-square terminology.
  constexpr auto depth() const noexcept -> std::size_t {
    if (this->is_leaf())
      return 4;
    else
        return node.nw->
  }

private:
  /// A discriminated union is used to create a quadtree. The sentinel value is
  /// hidden as the first pointer in the object, though this does mean that we
  /// throw type safety out the window.
  union {
    node node;
    leaf leaf;
  };
};
} // namespace life