#pragma once

#include <that/core/BasicTypes.hpp>
#include <algorithm>

namespace that
{
	template <class UInt>
	struct Range
	{
		using Index = UInt;
		Index begin = 0;
		Index len = 0;

		static constinit const Index NPos = Index(-1);

		template <class Q>
		constexpr bool operator==(Range<Q> const& o) const
		{
			return begin == o.begin && len == o.len;
		}

		constexpr bool containsRelaxed(Index i) const
		{
			return i >= begin && i <= (begin + len);
		}

		constexpr bool containsStict(Index i) const
		{
			return i >= begin && i < (begin + len);
		}

		constexpr bool contains(Index i) const
		{
			return containsStict(i);
		}

		template <class Q>
		constexpr bool contains(Range<Q> const& r) const
		{
			return r.begin >= begin && r.end() <= end();
		}

		constexpr Index clamp(Index index) const
		{
			return std::clamp(index, begin, end() - 1);
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

		template <class OtherInt>
			requires std::convertible_to<Index, OtherInt>
		constexpr Range<OtherInt> staticCastTo() const
		{
			return Range<OtherInt>{
				.begin = static_cast<OtherInt>(begin),
				.len = static_cast<OtherInt>(len),
			};
		}
	};

	using Range32u = Range<u32>;
	using Range64u = Range<u64>;
	using Range32i = Range<i32>;
	using Range64i = Range<i64>;
	using Range_st = Range<size_t>;
}