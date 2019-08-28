//===-- hashtable.h - Macrocell class definition ----------------*- C++ -*-===//
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
/// Implementation of a hashtable used to support memoization of macrocells.
///
//===----------------------------------------------------------------------===//

#pragma once

#include <unordered_map>
#include <iostream>

namespace life {
/// Hash function for any set of arguments.
/// Requires that the arguments be convertible to std::size_t
template <typename... Args> auto hash(Args &&... args) noexcept -> std::size_t {
  if constexpr (sizeof...(args) == 0) return 0;
  else {
    return hash_impl(args...);
  }
}

namespace {
template <typename Arg, typename... Args>
constexpr auto hash_impl(Arg&& arg, Args&& ... args) noexcept -> std::size_t {
  constexpr auto scalar = (1 << (sizeof...(args) + 1)) + 1;
  return scalar * arg + hash_impl(args...);
}

constexpr auto hash_impl() noexcept -> std::size_t { 
    return 0;
}
}

/// Allows memoization of objects whose construction is costly.
/// Constructor arguments are hashed to determine whether or not the
/// corresponding element is already present. Only if this is not the case,
/// the new element is constructed.
template <typename Element> class hashtable {
public:
  /// If not yet present, constructs and emplaces an element.
  /// Returns a reference to the inserted element, or if already present, to
  /// the already contained equivalent element.
  template <typename... Args> auto emplace(Args &&... args) {
    auto key = hash(args...);
    auto result = elements.try_emplace(key, args...);
    return *result.first;
  };

private:
  using key_type = std::size_t;
  std::unordered_map<key_type, Element> elements;
};
} // namespace life