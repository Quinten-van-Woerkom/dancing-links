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
/// Definition of the macrocell type representing squares of cells of
/// arbitrary size.
///
//===----------------------------------------------------------------------===//

#pragma once

#include <cstdint>
#include <vector>

#include "hashtable.h"
#include "rules.h"

namespace life {
class node;
class leaf;

/// The universe is stored in terms of squares with edges multiples of 2 in
/// size. Their state is stored in the form of references to smaller,
/// canonicalized squares, as a quadtree.
/// Only the state of 8x8 squares is stored directly.
class macrocell {
public:
  /// Factory function that canonicalizes all macrocells.
  static auto create(macrocell nw, macrocell ne, macrocell sw, macrocell se);

  /// Returns the center N-1 square's future 2^(N-2) steps into the future.
  auto future() const -> macrocell;

  /// Returns the center N-1 square's future 1 step into the future.
  auto next() const -> macrocell;

  /// Returns the macrocell's level in the quadtree.
  auto level() const -> std::size_t;

  /// Returns true if all contained cells are dead.
  auto empty() const -> bool { return pointer == nullptr; };

  /// Type discriminators
  /// @{
  auto is_leaf() const -> bool { return level() == 4; };
  auto is_leaf_parent() const -> bool { return level() == 5; };
  auto is_node() const -> bool { return level() >= 5; };
  /// @}

  /// Implicit conversion to std::uintptr_t is required for hashing
  /// @{
  constexpr operator std::uintptr_t() const noexcept {
    return raw;
  };
  /// @}

private:
  /// Constructor is private to force memoization
  /// @{
  macrocell(macrocell nw, macrocell ne, macrocell sw, macrocell se);
  /// @}

  /// A macrocell is effectively a discriminated union of a node and leaf.
  /// The first field of both types is an integer denoting the depth of the
  /// object's children, also acting as discriminator between both types.
  union {
    std::uintptr_t raw;     // Raw byte data
    void *pointer;          // Raw pointer access
    node *node;
    leaf *leaf;
  };
};


/// Nodes store their states as pointers to other nodes.
/// Empty squares of any size are stored as null pointers, since their future
/// is always empty anyway.
class node {
public:
  /// Returns the node's level in the quadtree.
  auto level() const -> std::size_t { return this->depth; };

  /// Construction is best done through factory functions, as that allows for
  /// memoization.
  /// @{
  static auto create(node *nw, node *ne, node *sw, node *se) -> node;
  static auto create(leaf *nw, leaf *ne, leaf *sw, leaf *se) -> node;
  /// @}

  /// Constructors immediately populate the next and future fields.
  /// This is relatively expensive, so it is best not to call the constructors
  /// directly unless absolutely required.
  /// Constructors must be public to support hashtable usage.
  /// @{
  node(node *nw, node *ne, node *sw, node *se);
  node(leaf *nw, leaf *ne, leaf *sw, leaf *se);
  /// @}

private:
  /// All nodes are memoized when constructed using create(), to allow caching
  /// of futures.
  static inline hashtable<node> nodes;

  std::size_t depth;
  macrocell nw, ne, sw, se;
  macrocell next;
  macrocell future;
};


/// Leaf macrocells (8x8) directly store their cell states.
/// Futures are calculated directly.
class leaf {
public:
  /// Factory function that invokes the constructor and caches the result,
  /// allowing for memoization of leaves.
  static auto create(square<4> nw, square<4> ne, square<4> sw, square<4> se) -> leaf;
  static auto create(square<8> cells) -> leaf { return leaves.emplace(cells); };

  /// Constructors immediately populate the next and future fields, as
  /// evaluation is relatively inexpensive.
  /// Should not be called directly, but must be public to allow the hashtable
  /// to access it.
  /// @{
  leaf(square<4> nw, square<4> ne, square<4> sw, square<4> se);
  leaf(square<8> cells);
  /// @}

  /// Returns the leaf's discriminator value and depth in the quadtree.
  auto level() const -> std::size_t { return this->depth; };

  /// returns the leaf's state 2 generations into the future.
  auto future() const -> square<4> { return this->cached_future; };

  /// Returns the leaf's state 1 generation into the future.
  auto next() const -> square<6> { return this->cached_next; };

private:
  /// All nodes are memoized when constructed using create(), to allow caching
  /// of futures.
  static inline hashtable<leaf> leaves;

  const std::size_t depth = 3;
  square<8> cells;
  square<6> cached_next;
  square<4> cached_future;
};
} // namespace life