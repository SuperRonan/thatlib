#pragma once
#include <that/core/Core.hpp>
#include "../SegmentsUnion.hpp"

#include <algorithm>
#include <cassert>

namespace that
{
	template <impl::SegementScalar U, template <class T> class Container>
	void SegmentUnion<U, Container>::add_impl(Segment const& s)
	{
		using It = typename MyContainer::iterator;
		const Segment ws = s.widenToAdjacent();
		// The first it which intersects or touches segment s, or is beyond
		It lower = std::lower_bound(_segments.begin(), _segments.end(), ws.lower(), [](Segment const lhs, U segment_begin)
		{
			return lhs.upper() < segment_begin;
		});

		if (lower == _segments.end()) // Insert after
		{
			_segments.push_back(s);
			return;
		}
		else if (lower->lower() > s.upper()) // Insert before
		{
			_segments.insert(lower, s);
			return;
		}
		if (lower->upper() >= s.upper()) // Only extent lower
		{
			if (s.lower() < lower->lower()) // IFN
			{
				lower->lower() = s.lower();
			}
			return;
		}

		// The last it which intersects with segment s
		It upper = std::prev(std::lower_bound(std::next(lower), _segments.end(), ws.upper(), [](Segment const& lhs, U segment_end)
		{
			return lhs.lower() <= segment_end;
		}));

		assert(upper != _segments.end());
		assert(upper->intersects(ws));
		assert([&]() -> bool {
			const It next = std::next(upper);
			const bool res = (next == _segments.end()) || !next->intersects(ws);
			assert(res);
			return res;
		}());
		if (upper == lower)
		{
			lower->upper() = s.upper();
		}
		else
		{
			lower->upper() = upper->upper();
			_segments.erase(std::next(lower), std::next(upper));
		}
	}

	template <impl::SegementScalar U, template <class T> class Container>
	void SegmentUnion<U, Container>::add(Segment const& s)
	{
		if (s.len() == 0) return;
		assert(s.isSortedStrict()); // "No overflow / ring allowed!"
		add_impl(s);
		assert(checkInvariant());
	}

	template <impl::SegementScalar U, template <class T> class Container>
	void SegmentUnion<U, Container>::addKeepUnique(Segment const& s)
	{
		if (_segments.empty())
		{
			_segments.push_back(s);
		}
		else
		{
			assert(_segments.size() == 1);
			_segments[0] |= s;
		}
		assert(checkInvariant());
	}

	template <impl::SegementScalar U, template <class T> class Container>
	bool SegmentUnion<U, Container>::checkInvariant() const
	{
		if (_segments.empty())
		{
			return true;
		}
		bool res = true;
		auto it = _segments.begin();
		const auto end = _segments.end();
		while (it != end)
		{
			Segment const& s = *it;
			if (!s.isSortedRelaxed())
			{
				THAT_BREAKPOINT_HANDLE;
				res &= false;
			}
			const auto next = std::next(it);
			if (next != end)
			{
				Segment const& n = *next;
				if (Segment::CanMerge(s.upper(), n.lower()))
				{
					THAT_BREAKPOINT_HANDLE;
					res &= false;
				}
			}
			it = next;
		}
		return res;
	}

	template <impl::SegementScalar U, template <class T> class Container>
	template <class Stream>
	void SegmentUnion<U, Container>::printTo(Stream& stream) const
	{
		stream << "{";
		auto it = cbegin();
		const auto end = cend();
		while(it != end)
		{
			stream << *it;
			auto next = std::next(it);
			if (next != end)
			{
				stream << ", ";
			}
			it = next;
		}
		stream << "}";
	}
}