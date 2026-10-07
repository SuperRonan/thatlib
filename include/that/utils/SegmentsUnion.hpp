#pragma once

#include "Segment.hpp"
#include <vector>

namespace that
{
	template <impl::SegementScalar U = size_t, template <class T> class Container = std::vector>
	class SegmentUnion
	{
	public:

		using Segment = Segment<U>;
		using MyContainer = Container<Segment>;

	protected:

		// Sorted and mutualy exclusive
		MyContainer _segments;

		void add_impl(Segment const& s);

	public:

		constexpr SegmentUnion() noexcept = default;
		constexpr SegmentUnion(SegmentUnion&&) noexcept = default;
		constexpr SegmentUnion(SegmentUnion const&) = default;

		SegmentUnion& operator=(SegmentUnion const&) = default;
		SegmentUnion& operator=(SegmentUnion&&) noexcept = default;

		constexpr ~SegmentUnion() = default;

		void add(Segment const& s);

		void addKeepUnique(Segment const& s);

		bool checkInvariant() const;

		decltype(auto) begin() const
		{
			return _segments.begin();
		}

		decltype(auto) end() const
		{
			return _segments.end();
		}

		decltype(auto) cbegin() const
		{
			return _segments.cbegin();
		}

		decltype(auto) cend() const
		{
			return _segments.cend();
		}

		bool empty() const
		{
			return _segments.empty();
		}

		size_t size() const
		{
			return _segments.size();
		}

		void clear()
		{
			_segments.clear();
		}

		Segment outerBounds() const
		{
			if (empty())
			{
				return Segment{};
			}
			return Segment(_segments.front().lower(), _segments.back().upper());
		}

		template <class Stream>
		void printTo(Stream&) const;
	};
}

template <class Stream, that::impl::SegementScalar U, template <class T> class Container>
Stream& operator<<(Stream& stream, that::SegmentUnion<U, Container> const& s)
{
	s.printTo(stream);
	return stream;
}