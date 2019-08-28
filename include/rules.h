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
/// Provides the general-case life rules used for calculation of future cell
/// generations, using Gosper's algorithm to 'compress' time.
/// Additionally, bitwise operations are provided for efficient,
/// single-register calculation of next generations of an 8x8 square of life
/// cells, based on LIAR (Life in a Register): http://dotat.at/prog/life/liar.c
///
//===----------------------------------------------------------------------===//

#pragma once

#include <cstdint>

namespace life {
//===-- Future of 8x8 cell bitmap -----------------------------------------===//
using bitmap = std::uint64_t;

/// Returns the bit located at a given index.
template <typename Unsigned>
constexpr auto bit(Unsigned value, std::size_t index) noexcept -> bool {
  return static_cast<bool>((value >> index) & Unsigned(1));
}

/// Parallel half adder adding the bits of two unsigned integers.
/// Returns two unsigneds representing the resulting sum and carry bits.
template <typename Unsigned>
constexpr auto half_adder(Unsigned a, Unsigned b) noexcept {
  const Unsigned sum = a ^ b;
  const Unsigned carry = a & b;
  return std::pair{sum, carry};
}

/// Parallel full adder simultaneously adding the bits of three unsigned
/// integers. Returns two unsigneds representing the result and any carry bits,
/// respectively.
template <typename Unsigned>
constexpr auto full_adder(Unsigned a, Unsigned b, Unsigned c) noexcept {
  const Unsigned sum = a ^ b ^ c;
  const Unsigned carry = (a & b) | (b & c) | (a & c);
  return std::pair{sum, carry};
}

/// Returns the next generation of a bitmap of cells.
constexpr auto next(bitmap cells) noexcept -> bitmap {
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

/// Returns the generation 2 steps into the future of a bitmap.
constexpr auto future(bitmap cells) noexcept -> bitmap {
  return next(next(cells)) & 0x00003c3c3c3c0000;
}

//===-- Future generations ------------------------------------------------===//
/// Constructs the center square by taking the halves of two horizontally
/// adjacent squares.
template <typename Square>
constexpr auto horizontal_center(Square left, Square right) noexcept -> Square {
  return Square(left.ne, left.se, right.nw, right.sw);
}

/// Constructs the center square by taking the halves of two vertically
/// adjacent squares.
template <typename Square>
constexpr auto vertical_center(Square upper, Square lower) noexcept -> Square {
  return Square(upper.sw, upper.se, lower.nw, lower.ne);
}

/// Combines four bitmaps forming a square into a bitmap representing the
/// center quarter.
template <typename Square>
constexpr auto center(Square nw, Square ne, Square sw, Square se) noexcept
    -> Square {
  return Square(nw.se, ne.sw, sw.ne, se.nw);
}

/// Calculates the result of an n-square 2^(n-3) generations in advance.
template <typename Square>
constexpr auto compute_result(Square nw, Square ne, Square sw,
                              Square se) noexcept -> Square {
  const auto nw1 = nw.result();
  const auto nn1 = horizontal_center(nw, ne).result();
  const auto ne1 = ne.result();
  const auto ww1 = vertical_center(nw, sw).result();
  const auto cc1 = center(nw, ne, sw, se).result();
  const auto ee1 = vertical_center(ne, se).result();
  const auto sw1 = sw.result();
  const auto ss1 = horizontal_center(sw, se).result();
  const auto se1 = se.result();

  const auto nw2 = Square(nw1, nn1, ww1, cc1).result();
  const auto ne2 = Square(nn1, ne1, cc1, ee1).result();
  const auto sw2 = Square(ww1, cc1, sw1, ss1).result();
  const auto se2 = Square(cc1, ee1, ss1, se1).result();

  return Square(nw2, ne2, sw2, se2);
}
} // namespace life