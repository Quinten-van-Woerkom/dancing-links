//===-- rules.h - Fundamental life rules ------------------------*- C++ -*-===//
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
/// Provides the life rules for an 8x8 square of cells, the base building block
/// of this implementation's life universe.
/// Implemented in terms of bitwise operations for efficient, single-register
/// calculation of next generations.
/// Based on LIAR (Life in a Register): http://dotat.at/prog/life/liar.c
///
//===----------------------------------------------------------------------===//

#pragma once

#include <cstdint>
#include <tuple>

namespace life {
//===-- Supporting bit-twiddling functions --------------------------------===//
/// Returns the bit located at a given index.
constexpr auto bit(const std::uint64_t value, std::size_t index) noexcept -> bool {
  return static_cast<bool>((value >> index) & 1u);
}

/// Parallel half adder adding the bits of two unsigned integers.
/// Returns two unsigneds representing the resulting sum and carry bits.
constexpr auto half_adder(const std::uint64_t a, const std::uint64_t b) noexcept {
  const auto sum = a ^ b;
  const auto carry = a & b;
  return std::pair{sum, carry};
}

/// Parallel full adder simultaneously adding the bits of three unsigned
/// integers. Returns two unsigneds representing the result and any carry bits,
/// respectively.
constexpr auto full_adder(const std::uint64_t a, const std::uint64_t b,
                          const std::uint64_t c) noexcept {
  const auto sum = a ^ b ^ c;
  const auto carry = (a & b) | (b & c) | (a & c);
  return std::pair{sum, carry};
}

//===-- Square of life cells ----------------------------------------------===//
/// Bitmap representing an 8x8, 6x6, or 4x4 square of cells.
template <std::size_t size = 8> class square {
  static_assert(size == 8 || size == 6 || size == 4, "Unsupported square size");
public:
  /// Direct construction from an unsigned integer
  constexpr square(std::uint64_t other = 0u) noexcept : cells{other} {
    // Sets edge cells to zero for squares smaller than 8x8
    if constexpr (size == 6)
      cells &= 0x007e7e7e7e7e7e00;
    if constexpr (size == 4)
      cells &= 0x00003c3c3c3c0000;
    if constexpr (size == 2)
      cells &= 0x0000001818000000;
  }

  /// Directly constructs a square from four quadrants.
  /// Only supported for 8x8 and 4x4 squares.
  constexpr square(square<size / 2> nw, square<size / 2> ne,
                   square<size / 2> sw, square<size / 2> se,
                   typename std::enable_if<size == 8 || size == 4> * = 0) {
    constexpr int offset = size / 4;
    cells = nw.shift(-offset, -offset) | ne.shift(offset, -offset) |
            sw.shift(-offset, offset) | se.shift(offset, offset);
  }

  /// Returns the next generation of cells.
  constexpr auto next() const noexcept -> square<size - 2> {
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
    return result;
  }

  /// Returns the bitmap shifted down and to the right by the specified amounts.
  constexpr auto shift(int right, int down) const noexcept -> square {
    auto shifted = right + 8 * down;
    if (shifted > 0)
      return square{cells << shifted};
    else
      return square{cells >> -shifted};
  }

  /// Sets the cell at <index> to either alive (true) or dead (false).
  constexpr void set(std::size_t index, bool alive = true) noexcept {
    cells = (cells & ~(1ull << index)) | (uint64_t(alive) << index);
  }

  /// Sets the cell at (x, y) to either alive or dead.
  constexpr void set(std::size_t x, std::size_t y, bool alive = true) noexcept {
    this->set(x + 8 * y, alive);
  }

  /// Returns a centered subsquare of the current bitmap
  template <int subsize> constexpr auto subsquare() const noexcept -> square {
    return square<subsize>{cells};
  }

  /// Accessors returning only single quarters of the bitmap, shifted to be in
  /// the center.
  /// @{
  constexpr auto nw() const noexcept -> std::enable_if<size == 8, square<4>> {
    return this->shift(2, 2).subsquare<4>();
  }

  constexpr auto ne() const noexcept -> std::enable_if<size == 8, square<4>> {
    return this->shift(-2, 2).subsquare<4>();
  }

  constexpr auto sw() const noexcept -> std::enable_if<size == 8, square<4>> {
    return this->shift(2, -2).subsquare<4>();
  }

  constexpr auto se() const noexcept -> std::enable_if<size == 8, square<4>> {
    return this->shift(-2, -2).subsquare<4>();
  }
  /// @}

  /// The bitmap can be converted implicitly to its 64-bit unsigned
  /// representation.
  constexpr operator const std::uint64_t &() const noexcept {
    return this->cells;
  };
  constexpr operator std::uint64_t &() noexcept { return this->cells; };

private:
  std::uint64_t cells;
};
} // namespace life