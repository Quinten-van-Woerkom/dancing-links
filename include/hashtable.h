//===-- hashtable.h - hashtable class definition ----------------*- C++ -*-===//
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
/// Implementation of the hashtable utilised in canonisation of nodes.
///
//===----------------------------------------------------------------------===//

#include <unordered_set>

namespace life {
class macrocell;

template<typename T> constexpr auto hash(T *nw, T *ne, T *sw, T *se) noexcept {
  return 0x10001ull * reinterpret_cast<std::uint64_t>(nw) +
         0x1001ull * reinterpret_cast<std::uint64_t>(ne) +
         0x101ull * reinterpret_cast<std::uint64_t>(sw) +
         0x11ull * reinterpret_cast<std::uint64_t>(se);
};

/// Hashtable is accessed only through use of four pointers to cell squares.
class hash_table {
public:
    template<typename T> auto find(T* nw, T* ne, T* sw, T* se) {
    return squares.emplace(nw, ne, sw, se);
  }

private:
  std::unordered_set<macrocell, hash<macrocell>> squares;
};
} // namespace life