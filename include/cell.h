//===-- cell.h - Macrocell class definition ---------------------*- C++ -*-===//
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

#include <cstdint>
#include <tuple>

#include "macrocell.h"

namespace life {
class macrocell;

namespace {
//===-- Bitmap result combination -----------------------------------------===//
/// Combines two horizontally adjacent bitmaps into a bitmap representing
/// the center halves.
constexpr auto horizontal_center(bitmap left, bitmap right) noexcept -> bitmap {
  return bitmap(left.ne(), left.se(), right.nw(), right.sw());
}

/// Combines two vertically adjacent bitmaps into a bitmap representing the
/// center halves.
constexpr auto vertical_center(bitmap upper, bitmap lower) noexcept -> bitmap {
  return bitmap(upper.sw(), upper.se(), lower.nw(), lower.ne());
}

/// Combines four bitmaps forming a square into a bitmap representing the
/// center quarter.
constexpr auto center(bitmap nw, bitmap ne, bitmap sw, bitmap se) noexcept
    -> bitmap {
  return bitmap(nw.se(), ne.sw(), sw.ne(), se.nw());
}

/// Calculates the result of a leaf node 4 generations in advance.
/// This is not memoized due to the low cost of evaluation.
constexpr auto compute_result(bitmap nw, bitmap ne, bitmap sw, bitmap se) noexcept
    -> bitmap {
  const auto nw1 = nw.result();
  const auto nn1 = horizontal_center(nw, ne).result();
  const auto ne1 = ne.result();
  const auto ww1 = vertical_center(nw, sw).result();
  const auto cc1 = center(nw, ne, sw, se).result();
  const auto ee1 = vertical_center(ne, se).result();
  const auto sw1 = sw.result();
  const auto ss1 = horizontal_center(sw, se).result();
  const auto se1 = se.result();

  const auto nw2 = bitmap(nw1, nn1, ww1, cc1).result();
  const auto ne2 = bitmap(nn1, ne1, cc1, ee1).result();
  const auto sw2 = bitmap(ww1, cc1, sw1, ss1).result();
  const auto se2 = bitmap(cc1, ee1, ss1, se1).result();

  return bitmap(nw2, ne2, sw2, se2);
}


//===-- Leaf result combination -------------------------------------------===//
/// The underlying cells of leaf nodes are stored directly as bitmaps.
struct leaf_type {
  void *sentinel;
  bitmap nw, ne, sw, se;

  constexpr leaf_type(bitmap nw, bitmap ne, bitmap sw, bitmap se)
      : sentinel{nullptr}, nw{nw}, ne{ne}, sw{sw}, se{se} {};

  constexpr auto result() const noexcept -> bitmap {
    return compute_result(nw, ne, sw, se);
  }
};

/// Combines two horizontally adjacent leaf nodes into a leaf node representing
/// the center halves.
constexpr auto horizontal_center(leaf_type left, leaf_type right) noexcept
    -> leaf_type {
  return leaf_type(left.ne, left.se, right.nw, right.sw);
}

/// Combines two vertically adjacent leaf nodes into a leaf node representing
/// the center halves.
constexpr auto vertical_center(leaf_type upper, leaf_type lower) noexcept
    -> leaf_type {
  return leaf_type(upper.sw, upper.se, lower.nw, lower.ne);
}

/// Combines four leaf nodes into a single leaf node representing the center
/// quarter.
constexpr auto center(leaf_type nw, leaf_type ne, leaf_type sw,
    leaf_type se) noexcept -> leaf_type {
  return leaf_type(nw.se, ne.sw, sw.ne, se.nw);
}

/// Calculates the result of a leaf parent 8 generations in advance.
/// This is not memoized due to the low cost of evaluation.
constexpr auto compute_result(leaf_type nw, leaf_type ne, leaf_type sw,
    leaf_type se) noexcept -> leaf_type {
  const auto nw1 = nw.result();
  const auto nn1 = horizontal_center(nw, ne).result();
  const auto ne1 = ne.result();
  const auto ww1 = vertical_center(nw, sw).result();
  const auto cc1 = center(nw, ne, sw, se).result();
  const auto ee1 = vertical_center(ne, se).result();
  const auto sw1 = sw.result();
  const auto ss1 = horizontal_center(sw, se).result();
  const auto se1 = se.result();

  const auto nw2 = leaf_type(nw1, nn1, ww1, cc1).result();
  const auto ne2 = leaf_type(nn1, ne1, cc1, ee1).result();
  const auto sw2 = leaf_type(ww1, cc1, sw1, ss1).result();
  const auto se2 = leaf_type(cc1, ee1, ss1, se1).result();

  return leaf_type(nw2, ne2, sw2, se2);
}


//===-- Node result combination -------------------------------------------===//
/// Nodes store four pointer to their children, as well as a single pointer
/// to their result, if calculated.
struct node_type {
  macrocell *nw, *ne, *sw, *se;
  macrocell *result;

  constexpr node_type(macrocell *nw, macrocell *ne, macrocell *sw,
                      macrocell *se)
      : nw{nw}, ne{ne}, sw{sw}, se{se}, result{nullptr} {};

  constexpr node_type(node_type *nw, node_type *ne, node_type *sw,
                      node_type *se)
      : nw{reinterpret_cast<macrocell *>(nw)},
        ne{reinterpret_cast<macrocell *>(ne)}, sw{reinterpret_cast<macrocell *>(
                                                   sw)},
        se{reinterpret_cast<macrocell *>(se)}, result{nullptr} {};
};

/// Combines two horizontally adjacent nodes into a leaf node representing the
/// center halves.
constexpr auto horizontal_center(node_type left, node_type right) noexcept
    -> node_type {
  return node_type{left.ne, left.se, right.nw, right.sw, nullptr};
}

/// Combines two vertically adjacent leaf nodes into a node representing the
/// center halves.
constexpr auto vertical_center(node_type upper, node_type lower) noexcept
    -> node_type {
  return node_type{upper.sw, upper.se, lower.nw, lower.ne, nullptr};
}

/// Combines four nodes into a single node representing the center quarter.
constexpr auto center(node_type nw, node_type ne, node_type sw,
                      node_type se) noexcept -> node_type {
  return node_type{nw.se, ne.sw, sw.ne, se.nw, nullptr};
}

/// Calculates the result of a node.
constexpr auto compute_result(node_type nw, node_type ne, node_type sw,
                              node_type se) noexcept -> node_type {
  const auto nw1 = nw.result();
  const auto nn1 = horizontal_center(nw, ne).result();
  const auto ne1 = ne.result();
  const auto ww1 = vertical_center(nw, sw).result();
  const auto cc1 = center(nw, ne, sw, se).result();
  const auto ee1 = vertical_center(ne, se).result();
  const auto sw1 = sw.result();
  const auto ss1 = horizontal_center(sw, se).result();
  const auto se1 = se.result();

  const auto nw2 = node_type{nw1, nn1, ww1, cc1, nullptr}.result();
  const auto ne2 = node_type{nn1, ne1, cc1, ee1, nullptr}.result();
  const auto sw2 = node_type{ww1, cc1, sw1, ss1, nullptr}.result();
  const auto se2 = node_type{cc1, ee1, ss1, se1, nullptr}.result();

  return node_type{nw2, ne2, sw2, se2, nullptr};
}
} // namespace


/// A macrocell is the basic building block of the Life world, encompassing a
/// square of cells of arbitary size.
class macrocell {
public:
  /// Leaf nodes are discriminated using a sentinel value, which is null only
  /// for a leaf.
  /// @{
  constexpr auto is_node() const noexcept -> bool { return leaf.sentinel; }
  constexpr auto is_leaf() const noexcept -> bool { return !leaf.sentinel; }
  /// @}

  /// Parents of leaf nodes are special-cased since the calculation of their
  /// result must be executed differently from both other nodes and leafs.
  constexpr auto is_leaf_parent() const noexcept -> bool {
    if (is_leaf())
      return false;
    return node.nw->is_leaf();
  }

  /// Returns the number of underlying cell layers.
  /// Leaf nodes are 16x16 cells; for consistency with hashlife convention,
  /// their depth is taken to be 4.
  constexpr auto depth() const noexcept -> std::size_t {
    // Current implementation recurses down to leaf nodes.
    // It might be useful to consider calculating depth at construction and
    // storing the result, at the expense of memory.
    if (is_leaf())
      return 4;
    else
      return node.nw->depth() + 1;
  }

private:
  /// Macrocells are implemented as quadtrees.
  union {
    static_assert(sizeof(node_type) == sizeof(leaf_type),
                  "Leaf and node size not equal");
    node_type node;
    leaf_type leaf;
  };
};
} // namespace life