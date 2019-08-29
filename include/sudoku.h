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
/// Data structures used in solving a sudoku.
///
//===----------------------------------------------------------------------===//

#pragma once

#include <array>
#include <bitset>
#include <cassert>
#include <cstddef>

#if defined(_MSC_VER)
/// Required for built-in popcount
#include <intrin.h>
#endif

namespace sudoku {
/// A cell stores its possible values.
class cell {
public:
  constexpr cell(){};

  /// Initialises the cell to have possibilities <values>.
  template<typename... Values>
  constexpr cell(Values... values) {
    possibilities = 0;
    add(values);
  }

  /// Adds <value> as possible alternative.
  constexpr void add(std::size_t value) {
    assert(value >= 1 && value <= 9 && "Only 1-9 are valid cell values.");
    possibilities |= 1 << (value - 1);
  }

  /// Adds <values> as possible alternatives.
  template<typename... Values>
  constexpr void add(std::size_t value, Values... values) {
    add(value);
    add(values...);
  }

  /// Prunes <value> from the list of alternatives.
  constexpr void prune(std::size_t value) {
    assert(value >= 1 && value <= 9 && "Only 1-9 are valid cell values.");
    possibilities &= ~(1 << (value - 1));
  }

  /// Prunes <values> from the list of alternatives.
  template<typename... Values>
  constexpr void prune(std::size_t value, Values... values) {
    prune(value);
    prune(values...);
  }

  /// Sets <value> to be the only possible alternative.
  constexpr void solve(std::size_t value) {
    assert(value >= 1 && value <= 9 && "Only 1-9 are valid cell values.");
    possibilities = 1 << (value - 1);
  }

  /// Whether or not the cell has only one alternative left.
  auto solved() const noexcept -> bool { return count() == 1; };

  /// Returns the solution, assuming that the cell has been solved (!).
  /// Tries intrinsics, but if that is not possible, falls back to using a
  /// De Bruijn sequence.
  auto solution() const noexcept -> std::size_t {
    assert(solved() && "Trying to obtain solution from unsolved cell");
#if defined(__clang__)
    return __builtin_clz(possibilities) + 1;
#elif defined(__GNUC__) || defined(__GNUG__)
    return __builtin_ffs(possibilities);
#elif defined(_MSC_VER)
    unsigned long index = 0;
    _BitScanForward(&index, possibilities);
    return index + 1u;
#else
    // From http://graphics.stanford.edu/~seander/bithacks.html
    constexpr auto de_bruijn_sequence = std::array{
        0,  1,  28, 2,  29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4,  8,
        31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6,  11, 5,  10, 9};
    return de_bruijn_sequence[((uint32_t)((possibilities & -possibilities) * 0x077CB531U)) >> 27];
#endif
  }

  /// Returns the number of possibilities still remaining.
  /// Tries intrinsics, but falls back to a bit twiddling hack if necessary.
  auto count() const noexcept -> std::size_t {
#if defined(__clang__)
    return __builtin_popcount(possibilities);
#elif defined(__GNUC__) || defined(__GNUG__)
    return __builtin_popcount(possibilities);
#elif defined(_MSC_VER)
    return __popcnt(possibilities);
#else
    // From http://graphics.stanford.edu/~seander/bithacks.html
    return (possibilities * 0x200040008001ULL & 0x111111111111111ULL) % 0xf;
#endif
  }

private:
  unsigned long possibilities = 0b111111111;
};
} // namespace sudoku