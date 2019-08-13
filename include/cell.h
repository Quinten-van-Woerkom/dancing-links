#pragma once

#include <cstdint>
#include <iostream>
#include <string_view>
#include <tuple>

#include "bit.h"

namespace life {
/**
 *	A macrocell is a union of node and leaf cells, discriminated by the value of the nw child.
 *	When nw is null, the macrocell is a leaf cell; otherwise, it is a node.
 */
union macrocell {
private:
	struct node_type {
		macrocell* nw, * ne, * sw, * se;
		macrocell* result;
	};

	struct leaf_type {
		macrocell* sentinel;
		bitmap nw, ne, sw, se;
	};

	static_assert(sizeof(node_type) == sizeof(leaf_type), "Leaf and node size not equal");

	node_type node;
	leaf_type leaf;

public:
	constexpr auto is_node() const noexcept -> bool {
		return leaf.sentinel;
	}

	constexpr auto is_leaf() const noexcept -> bool {
		return !leaf.sentinel;
	}

	constexpr auto is_leaf_parent() const noexcept -> bool {
		if (is_leaf()) return false;
		return node.nw->is_leaf();
	}
};
}