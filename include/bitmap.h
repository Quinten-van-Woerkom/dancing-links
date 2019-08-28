//===-- bitmap.h - Cell bitmap ----------------------------------*- C++ -*-===//
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
/// Behaviour of cell squares 8x8 and smaller is implemented as a bitmap.
///
//===----------------------------------------------------------------------===//

#pragma once

#include <cstdint>
#include <type_traits>

namespace life {
/// Implementation of a 64-bit bitmap representing an 8x8 square (or smaller subsquares).
template<int size>
class bitmap {
public:
  /// Direct construction from an unsigned integer
  constexpr bitmap(std::uint64_t other = 0u) noexcept : bits{other} {
    truncate_edges();
  }

  /// Returns the bitmap shifted down and to the right by the specified amounts.
  constexpr auto shift(int right, int down) const noexcept -> bitmap {
    auto shifted = right + 8 * down;
    if (shifted > 0)
      return bitmap{bits << shifted};
    else
      return bitmap{bits >> -shifted};
  }

  /// Sets the cell at <index> to either alive (true) or dead (false).
  constexpr void set(std::size_t index, bool alive = true) noexcept {
    bits = (bits & ~(1ull << index)) | (uint64_t(alive) << index);
  }

  /// Sets the cell at (x, y) to either alive or dead.
  constexpr void set(std::size_t x, std::size_t y, bool alive = true) noexcept {
    this->set(x + 8 * y, alive);
  }

  /// Returns a centered subsquare of the current bitmap
  template<int subsize>
  constexpr auto subsquare() const noexcept -> bitmap<subsize> {
    if constexpr (subsize == 6)
      return bits & 0x007e7e7e7e7e7e00;
    if constexpr (subsize == 4)
      return bits & 0x00003c3c3c3c0000;
    if constexpr (subsize == 2)
      return bits & 0x0000001818000000;
  }

  /// Sets the non-relevant bits of <bits> to 0, ensuring that the bitmap is
  /// in a valid state.
  constexpr auto truncate_edges() noexcept {
    if constexpr (size == 6) bits &= 0x007e7e7e7e7e7e00;
    if constexpr (size == 4) bits &= 0x00003c3c3c3c0000;
    if constexpr (size == 2) bits &= 0x0000001818000000;
  }

  /// Sets the edges to 0.
  constexpr auto mask_edges() const noexcept -> bitmap<size - 2> {
    if constexpr (size - 2 == 6)
      return bits & 0x007e7e7e7e7e7e00;
    if constexpr (size - 2 == 4)
      return bits & 0x00003c3c3c3c0000;
    if constexpr (size - 2 == 2)
      return bits & 0x0000001818000000;
  }

  /// Accessors returning only single quarters of the bitmap, shifted to be in
  /// the center.
  /// @{
  constexpr auto nw() const noexcept -> std::enable_if<size == 8, bitmap<4>> {
    return this->shift(2, 2).subsquare<4>();
  }

  constexpr auto ne() const noexcept -> std::enable_if<size == 8, bitmap<4>> {
    return this->shift(-2, 2).subsquare<4>();
  }

  constexpr auto sw() const noexcept -> std::enable_if<size == 8, bitmap<4>> {
    return this->shift(2, -2).subsquare<4>();
  }

  constexpr auto se() const noexcept -> std::enable_if<size == 8, bitmap<4>> {
    return this->shift(-2, -2).subsquare<4>();
  }
  /// @}

  /// The bitmap can be converted implicitly to its 64-bit unsigned
  /// representation.
  constexpr operator const std::uint64_t &() const noexcept {
    return this->bits;
  };
  constexpr operator std::uint64_t &() noexcept { return this->bits; };

protected:
  std::uint64_t bits;
};
} // namespace life
