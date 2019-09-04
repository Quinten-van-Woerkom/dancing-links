//===-- utility.h - Utility -------------------------------------*- C++ -*-===//
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
/// Small quality-of-life functions.
///
//===----------------------------------------------------------------------===//

#pragma once

#include <iterator>
#include <tuple>

namespace stx {
//===-- enumerate -------------------------------------------------------=====//
/// Enumeration over (index, element) pairs of an iterable container.
template <typename T,
          typename Iterator = decltype(std::begin(std::declval<T>())),
          typename = decltype(std::end(std::declval<T>()))>
constexpr auto enumerate(T &&iterable) {
  struct iterator {
    std::size_t index;
    Iterator current;

    bool operator!=(const iterator &other) { return current != other.current; }

    void operator++() {
      ++index;
      ++current;
    }

    auto operator*() { return std::tie(index, *current); }
  };

  struct wrapper {
    T iterable;
    auto begin() { return iterator{0, std::begin(iterable)}; }
    auto end() { return iterator{0, std::end(iterable)}; }
  };

  return wrapper{std::forward<T>(iterable)};
}

//===-- pairwise iteration ----------------------------------------------=====//
template <typename T,
          typename Iterator = decltype(std::begin(std::declval<T>())),
          typename = decltype(std::end(std::declval<T>()))>
constexpr auto pairwise(T &&iterable) {
  struct iterator {
    Iterator previous;
    Iterator current;

    bool operator!=(const iterator &other) { return current != other.current; }

    void operator++() {
      ++previous;
      ++current;
    }

    auto operator*() { return std::tie(*previous, *current); }
  };

  struct wrapper {
    T iterable;
    auto begin() { return iterator{std::begin(iterable), ++std::begin(iterable)}; }
    auto end() { return iterator{--std::end(iterable), std::end(iterable)}; }
  };

  return wrapper{std::forward<T>(iterable)};
}
} // namespace stx