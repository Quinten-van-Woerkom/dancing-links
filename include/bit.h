/**
 *  Provides bitwise operations useful for efficient, single-register calculation
 *	of next generation of an 8x8 square.
 *	Based on LIAR (Life in a Register): http://dotat.at/prog/life/liar.c
 */

#pragma once

#include <cstdint>
#include <tuple>

namespace life {
using std::uint64_t;
using std::size_t;

/**
 *	Returns the bit located at a given index.
 */
constexpr auto bit(uint64_t value, size_t index) noexcept -> bool {
    return static_cast<bool>((value >> index) & 1u);
}

/**
 *	Parallel half adder adding the bits of two 64-bit unsigned integers.
 *	returns two 64-bit unsigneds representing the resulting sum and carry bits.
 */
constexpr auto half_adder(uint64_t a, uint64_t b) noexcept {
	const uint64_t sum = a ^ b;
	const uint64_t carry = a & b;
	return std::pair{ sum, carry };
}

/**
 *	Parallel full adder simultaneously adding the bits of three 64-bit unsigned
 *	integers. Returns two 64-bit unsigneds representing the result and any
 *	carry bits, respectively.
 */
constexpr auto full_adder(uint64_t a, uint64_t b, uint64_t c) noexcept {
	const uint64_t sum = a ^ b ^ c;
	const uint64_t carry = (a & b) | (b & c) | (a & c);
	return std::pair{ sum, carry };
}

/**
 *	Interprets the 64-bit unsigned as an 8x8 bitmap of cells, and counts the
 *	number of living neighbours of each cell in the form of 3 bitmaps
 *	representing bit counts of 1, 2, and 4. Each cell is counted as its own
 *	neighbour, and counts of 8 and 9 wrap around to 0 and 1, for a faster
 *	implementation, as those counts are equivalent in life rules. A cell
 *	remains in the same state for a count of 4 and is alive for a count of 3;
 *	otherwise, it dies. Edge cells are returned as dead; information on their
 *	outer neighbours would be required to be certain on their next state.
 */
constexpr auto next_generation(uint64_t bitmap) noexcept -> uint64_t {
	const auto left = bitmap << 1;
	const auto right = bitmap >> 1;
	const auto [mid1, mid2] = full_adder(left, bitmap, right);

	const auto up1 = mid1 << 8;
	const auto up2 = mid2 << 8;
	const auto down1 = mid1 >> 8;
	const auto down2 = mid2 >> 8;

	const auto [sum1, sum2a] = full_adder(up1, mid1, down1);
	const auto [sum2b, sum4a] = full_adder(up2, mid2, down2);
	const auto [sum2, sum4b] = half_adder(sum2a, sum2b);
	const auto sum4 = sum4a ^ sum4b;

	const auto result = bitmap & (~sum1 & ~sum2 & sum4) | (sum1 & sum2 & ~sum4);
	return result;
}

/**
 *  Single cell bitmap, representing an 8x8 cell block.
 *  Cell states are stored as bits in a 64-bit unsigned integer, with the low
 *  bit representing the cell in the upper-left corner and the high bit
 *  representing the lower-right cell.
 */
class bitmap {
public:
	constexpr bitmap(std::string_view init) noexcept : cells{} {
		auto counter = 0;
		for (auto c : init) {
			if (c == '1') set(counter++, true);
			if (c == '0') set(counter++, false);
		}
	}

	constexpr bitmap(uint64_t cells = 0) noexcept : cells{ cells } {}

	constexpr bitmap(bitmap nw, bitmap ne, bitmap sw, bitmap se) noexcept : cells{} {
		cells |= nw.shift(-2, -2);
		cells |= ne.shift(2, -2);
		cells |= sw.shift(-2, 2);
		cells |= se.shift(2, 2);
	}

	/**
	 *  Provides grid-based indexing into the cell.
	 *  Upper left is (0, 0), lower right (7, 7).
	 */
	constexpr auto alive(size_t x, size_t y) const noexcept -> bool {
		return bit(cells, x + 8 * y);
	}

	/**
	 *  Calculates the next state for the central 6x6 square.
	 *  Edges are returned as 0: information on surrounding squares would be
	 *  necessary to calculate their values.
	 */
	constexpr auto next() const noexcept -> bitmap {
		return bitmap{ next_generation(this->cells) }.reset_boundaries();
	}

	/**
	 *	Calculates the state <generations> in advance.
	 *	Only valid for grids resulting in empty cells along the boundaries.
	 */
	constexpr auto next(size_t generations) const noexcept -> bitmap {
		auto result = bitmap{ *this };
		for (auto i = 0; i < generations; ++i) {
			result = result.next();
		}
		return result;
	}

	/**
	 *	Calculates the result of the bitmap, that is, its state 2 generations in advance.
	 *	Any cells outside the centre 4x4 square are set to 0, their future requires information
	 *	about external cells.
	 */
	constexpr auto result() const noexcept -> bitmap {
		return bitmap{ next_generation(next_generation(this->cells)) & 0x003c3c3c3c3c3c00 };
	}

	/**
	 *	Returns the cell shifted down and to the right by the specified amounts.
	 */
	constexpr auto shift(int right, int down) const noexcept -> bitmap {
		auto shifted = right + 8 * down;
		if (shifted > 0) return bitmap{ cells << shifted };
		else return bitmap{ cells >> -shifted };
	}

	/**
	 *	Sets the cell at <index> to either alive (true) or dead (false).
	 */
	constexpr void set(size_t index, bool alive = true) noexcept {
		cells = (cells & ~(1ull << index)) | (uint64_t(alive) << index);
	}

	/**
	 *	Sets the cell at (x, y) to either alive or dead.
	 */
	constexpr void set(size_t x, size_t y, bool alive = true) noexcept {
		this->set(x + 8 * y, alive);
	}

	/**
	 *	The bitmap can be converted implicitly to its 64-bit unsigned representation.
	 */
	constexpr operator const uint64_t& () const noexcept { return this->cells; };
	constexpr operator uint64_t& () noexcept { return this->cells; };

	/**
	 *	Sets the boundary cells to 0. This is useful to indicate that not enough
	 *	information on their neighbours to determine future states.
	 */
	constexpr auto reset_boundaries() const noexcept -> bitmap {
		return cells & 0x007e7e7e7e7e7e00;
	}

	/**
	 *	Accessors returning only single quarters of the bitmap, shifted to be in the center.
	 */
	constexpr auto nw() const noexcept -> bitmap {
		return shift(2, 2).reset_boundaries();
	}

	constexpr auto ne() const noexcept -> bitmap {
		return shift(-2, 2).reset_boundaries();
	}

	constexpr auto sw() const noexcept -> bitmap {
		return shift(2, -2).reset_boundaries();
	}

	constexpr auto se() const noexcept -> bitmap {
		return shift(-2, -2).reset_boundaries();
	}

private:
	uint64_t cells;
};

/**
 *	Combines two horizontally adjacent bitmaps into a bitmap representing the center halves.
 */
constexpr auto horizontal_center(bitmap left, bitmap right) noexcept -> bitmap {
	return (right.shift(4, 0) & 0xf0f0f0f0f0f0f0f0) | (left.shift(-4, 0) & 0x0f0f0f0f0f0f0f0f);
}

/**
 *	Combines two vertically adjacent bitmaps into a bitmap representing the center halves.
 */
constexpr auto vertical_center(bitmap upper, bitmap lower) noexcept -> bitmap {
	return (lower.shift(0, 4) & 0xffffffff00000000) | (upper.shift(0, -4) & 0x00000000ffffffff);
}

/**
 *	Combines four bitmaps forming a square into a bitmap representing the center quarter.
 */
constexpr auto center(bitmap nw, bitmap ne, bitmap sw, bitmap se) noexcept -> bitmap {
	return bitmap{ nw.se(), ne.sw(), sw.ne(), se.nw() };
}

/**
 *	Calculates the result of a square consisting of four bitmaps (a leaf parent).
 *	This is not memoized due to the low cost of evaluation.
 */
constexpr auto result(bitmap nw, bitmap ne, bitmap sw, bitmap se) noexcept -> bitmap {
	const auto nw1 = nw.result();
	const auto nn1 = horizontal_center(nw, ne).result();
	const auto ne1 = ne.result();
	const auto ww1 = vertical_center(nw, sw).result();
	const auto cc1 = center(nw, ne, sw, se).result();
	const auto ee1 = vertical_center(ne, se).result();
	const auto sw1 = sw.result();
	const auto ss1 = horizontal_center(sw, se).result();
	const auto se1 = se.result();

	const auto nw2 = bitmap{ nw1, nn1, ww1, cc1 }.result();
	const auto ne2 = bitmap{ nn1, ne1, cc1, ee1 }.result();
	const auto sw2 = bitmap{ ww1, cc1, sw1, ss1 }.result();
	const auto se2 = bitmap{ cc1, ee1, ss1, se1 }.result();

	return bitmap{ nw2, ne2, sw2, se2 };
}


auto& operator<<(std::ostream& os, bitmap other) {
	for (auto y = 0; y < 8; ++y) {
		for (auto x = 0; x < 8; ++x) {
			if (other.alive(x, y)) os << "\xdb\xdb";
			else os << "[]";
		}
		os << '\n';
	}
	return os;
}

namespace grids {
	constexpr auto empty = bitmap{ R"(
		00000000
		00000000
		00000000
		00000000
		00000000
		00000000
		00000000
		00000000
	)" };

	constexpr auto blinker = bitmap{ R"(
		00000000
		00000000
		00000000
		00111000
		00000000
		00000000
		00000000
		00000000
	)" };

	constexpr auto glider = bitmap{ R"(
		00000000
		00000000
		00001000
		00000100
		00011100
		00000000
		00000000
		00000000
	)" };

	constexpr auto toad = bitmap{ R"(
		00000000
		00000000
		00000000
		00011100
		00111000
		00000000
		00000000
		00000000
	)" };
}
}