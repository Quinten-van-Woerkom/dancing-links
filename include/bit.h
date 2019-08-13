/**
 *  Provides bitwise operations useful for calculation of single-cell states.
 */

#pragma once

#include <cstdint>

namespace life {
template<typename Integer>
constexpr auto bit(Integer value, size_t index) noexcept -> bool {
    return static_cast<bool>((value >> index) & 1u);
}
}