#pragma once

#include <stdint.h>

namespace that
{
	template <class UInt>
	struct Range
	{
		using Index = UInt;
		Index begin = 0;
		Index len = 0;

		template <class Q>
		constexpr bool operator==(Range<Q> const& o) const
		{
			return begin == o.begin && len == o.len;
		}

		constexpr bool contains(Index i) const
		{
			return i >= begin && i < (begin + len);
		}

		template <class Q>
		constexpr bool contains(Range<Q> const& r) const
		{
			return r.begin >= begin && r.end() <= end();
		}

		constexpr Index end()const
		{
			return begin + len;
		}

		// Assume ranges are valid
		constexpr Range& operator|=(Range const& o)
		{
			Index end = std::max(this->end(), o.end());
			begin = std::min(begin, o.begin);
			len = end - begin;
			return *this;
		}

		// Assume ranges are valid
		constexpr Range operator|(Range const& o) const
		{
			Range res = *this;
			res |= o;
			return res;
		}

		// Assume ranges are valid
		constexpr Range& operator|=(Index u)
		{
			Index end = std::max(this->end(), u);
			begin = std::min(begin, u);
			len = end - begin;
			return *this;
		}

		// Assume ranges are valid
		constexpr Range operator|(Index u) const
		{
			Range res = *this;
			res |= u;
			return res;
		}
	};

	using Range32u = Range<uint32_t>;
	using Range64u = Range<uint64_t>;
	using Range32i = Range<int32_t>;
	using Range64i = Range<int64_t>;
	using Range_st = Range<size_t>;
}