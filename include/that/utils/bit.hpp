#pragma once

#include <bit>
#include <concepts>
#include <that/core/BasicTypes.hpp>

namespace that
{
	// Until c++29 std::shl
	template <class I>
		requires requires(I i, unsigned int s) {
			{i << s} -> std::convertible_to<I>;
			{i >> s} -> std::convertible_to<I>;
		}
	I ShiftLeftI(I i, int shift_offset)
	{
		return shift_offset < 0 ? (i >> static_cast<uint>(-shift_offset)) : (i << static_cast<uint>(shift_offset));
	}

	// Until c++29 std::shr
	template <class I>
		requires requires(I i, unsigned int s) {
			{ i << s } -> std::convertible_to<I>;
			{ i >> s } -> std::convertible_to<I>;
	}
	I ShiftRightI(I i, int shift_offset)
	{
		return shift_offset < 0 ? (i << static_cast<uint>(-shift_offset)) : (i >> static_cast<uint>(shift_offset));
	}
}