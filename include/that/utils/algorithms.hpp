#pragma once

#include "bit.hpp"
#include <ranges>
#include <that/stl_ext/alignment.hpp>

namespace that
{
	template <std::ranges::input_range R, std::unsigned_integral U = typename std::remove_cvref<std::ranges::range_value_t<R>>::type>
		requires std::ranges::forward_range<R>
	static constexpr U GatherBitFlagsFromRange(R&& r, uint bit_index, uint bit_count = 1)
	{
		U res = {};
		uint counter = 0;
		const U mask = std::bitMask<U>(bit_count) << bit_index;
		for (auto it : r)
		{
			res |= ShiftLeftI((*it & mask), (counter * bit_count - bit_index));
			++counter;
		}
		return res;
	}
}