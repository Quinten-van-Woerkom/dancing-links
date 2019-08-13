#pragma once

#include <cstdint>
#include <tuple>

#include "bit.h"

namespace life {
/**
 *  Single cell bitmap, representing an 8x8 cell block.
 *  Cell states are stored as bits in a 64-bit unsigned integer, with the low
 *  bit representing the cell in the upper-left corner and the high bit
 *  representing the lower-right cell.
 */
class bitmap {
public:
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
        const auto left = cells << 1;
        const auto right = cells >> 1;
    }

private:
    std::uint64_t cells;

    static constexpr auto half_adder()
};
}