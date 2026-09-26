#pragma once

#include <bit>
#include <concepts>
#include <limits>
#include <that/core/BasicTypes.hpp>

namespace that
{
	namespace impl
	{
		template <class I>
		concept Shiftable = requires(I i, unsigned int s) {
			{ i << s } -> std::convertible_to<I>;
			{ i >> s } -> std::convertible_to<I>;
		};

		template <std::integral I>
		static constexpr I OverSHRValue(I i)
		{
			if constexpr (std::is_unsigned<I>::value)	return I(0);
			else if (i < 0)                             return I(-1);
			else	                                    return I(0);
		}
	}

	template <impl::Shiftable I>
	static constexpr I ShiftLeftUnchecked(I i, uint shift_offset)
	{
		return i << shift_offset;
	}

	template <impl::Shiftable I>
	static constexpr I ShiftRightUnchecked(I i, uint shift_offset)
	{
		return i >> shift_offset;
	}

	// Until c++29 std::shl
	template <impl::Shiftable I>
	static constexpr I ShiftLeftUnchecked(I i, int shift_offset)
	{
		return shift_offset < 0 ? ShiftRightUnchecked(i, static_cast<uint>(-shift_offset)) : ShiftLeftUnchecked(i, static_cast<uint>(shift_offset));
	}

	// Until c++29 std::shr
	template <impl::Shiftable I>
	static constexpr I ShiftRightUnchecked(I i, int shift_offset)
	{
		return shift_offset < 0 ? ShiftLeftUnchecked(i, static_cast<uint>(-shift_offset)) : ShiftRightUnchecked(i, static_cast<uint>(shift_offset));
	}

	// Until c++29 std::shl
	template <impl::Shiftable I>
	static constexpr I ShiftLeftChecked(I i, uint shift_offset)
	{
		constexpr const size_t width = std::numeric_limits<typename std::make_unsigned<I>::type>::digits;
		return shift_offset >= width ? I(0) : ShiftLeftUnchecked(i, shift_offset);
	}

	// Until c++29 std::shr
	template <impl::Shiftable I>
	static constexpr I ShiftRightChecked(I i, uint shift_offset)
	{
		constexpr const size_t width = std::numeric_limits<typename std::make_unsigned<I>::type>::digits;
		return shift_offset >= width ? impl::OverSHRValue(i) : ShiftRightUnchecked(i, shift_offset);
	}

	// Until c++29 std::shl
	template <impl::Shiftable I>
	static constexpr I ShiftLeftChecked(I i, int shift_offset)
	{
		return shift_offset < 0 ? ShiftRightChecked(i, static_cast<uint>(-shift_offset)) : ShiftLeftChecked(i, static_cast<uint>(shift_offset));
	}

	// Until c++29 std::shr
	template <impl::Shiftable I>
	static constexpr I ShiftRightChecked(I i, int shift_offset)
	{
		return shift_offset < 0 ? ShiftLeftChecked(i, static_cast<uint>(-shift_offset)) : ShiftRightChecked(i, static_cast<uint>(shift_offset));
	}
}