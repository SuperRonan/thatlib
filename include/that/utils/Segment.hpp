#pragma once

#include <that/core/BasicTypes.hpp>
#include <limits>
#include <array>
#include <that/core/Range.hpp>

namespace that
{
	namespace impl
	{
		struct SegmentBase
		{
			struct EmptyTag {};
		};

		template <class T>
		concept SegementScalar = std::arithmetic<T>;
	}
	// Lightweight AABB<1, T>
	// Bounds are preferably sorted ([lower, upper]), but user may choose not to
	// A segment is considered empty if lower > upper (lower >= upper in the strict sense)
	// A "pure" empty segment is the one constructed with EmptyTag
	template <impl::SegementScalar T>
	class Segment : public impl::SegmentBase
	{
	protected:
		std::array<T, 2> _bounds = {};
	public:

		using EmptyTag = impl::SegmentBase::EmptyTag;

		constexpr Segment() = default;

		constexpr Segment(EmptyTag) :
			_bounds({
				std::numeric_limits<T>::max(),
				std::numeric_limits<T>::lowest(),
			})
		{}

		constexpr Segment(T lower, T upper) :
			_bounds({ lower, upper })
		{}

		constexpr Segment(Segment const&) noexcept = default;
		constexpr Segment& operator=(Segment const&) noexcept = default;

		template <impl::SegementScalar Q>
		explicit(ScalarConversionExplicit<T, Q>::value)
		constexpr Segment(Segment<Q> const& other) :
			_bounds({ static_cast<T>(other.lower()), static_cast<T>(other.upper()) })
		{}

		template <impl::SegementScalar Q>
		constexpr Segment<Q> cast() const
		{
			return Segment<Q>(static_cast<Q>(lower()), static_cast<Q>(upper));
		}

		template <impl::SegementScalar Q>
		explicit(ScalarConversionExplicit<Q, T>::value)
		constexpr operator Segment<Q>() const
		{
			return cast<Q>();
		}

		constexpr std::array<T, 2> const& bounds() const
		{
			return _bounds;
		}

		constexpr std::array<T, 2>& bounds()
		{
			return _bounds;
		}

		constexpr T operator[](size_t index) const
		{
			return _bounds[index];
		}

		constexpr T& operator[](size_t index)
		{
			return _bounds[index];
		}

		constexpr T lower() const
		{
			return _bounds[0];
		}

		constexpr T upper() const
		{
			return _bounds[1];
		}

		constexpr T& lower()
		{
			return _bounds[0];
		}

		constexpr T& upper()
		{
			return _bounds[1];
		}

		constexpr bool containsStrict(T t) const
		{
			return t > lower() && t < upper();
		}

		constexpr bool containsRelaxed(T t) const
		{
			return t >= lower() && t <= upper();
		}

		constexpr bool contains(T t, bool strict = true) const
		{
			return strict ? containsStrict(t) : containsRelaxed(t);
		}

		constexpr T len() const
		{
			return upper() - lower();
		}

		constexpr bool isSortedRelaxed() const
		{
			return upper() >= lower();
		}

		constexpr bool isSortedStrict() const
		{
			return upper() > lower();
		}

		constexpr bool isSorted(bool strict = true) const
		{
			return strict ? isSortedStrict() : isSortedRelaxed();
		}

		constexpr const T* data() const
		{
			return _bounds.data();
		}

		constexpr T* data()
		{
			return _bounds.data();
		}

		constexpr bool operator==(Segment const& rhs) const
		{
			_bounds == rhs._bounds;
		}

		constexpr Segment& operator|=(Segment const& rhs)
		{
			_bounds[0] = std::min(_bounds[0], rhs[0]);
			_bounds[1] = std::max(_bounds[1], rhs[1]);
			return *this;
		}

		constexpr Segment operator|(Segment const& rhs) const
		{
			return Segment(std::min(_bounds[0], rhs[0]), std::max(_bounds[0], _bounds[1]));
		}

		constexpr Segment& operator&=(Segment const& rhs)
		{
			_bounds[0] = std::max(_bounds[0], rhs[0]);
			_bounds[1] = std::min(_bounds[1], rhs[1]);
			return *this;
		}

		constexpr Segment operator&(Segment const& rhs) const
		{
			return Segment(std::max(_bounds[0], rhs[0]), std::min(_bounds[0], _bounds[1]));
		}

		constexpr T clamp(T t) const
		{
			// assert(isSortedRelaxed()); // At your own risk
			return (t < lower()) ? lower() : ((t > upper()) ? upper() : t);
		}

		constexpr bool intersects(Segment const& rhs) const
		{
			return rhs.upper() >= lower() && rhs.lower() <= upper();
		}

		template <class Stream>
		void printTo(Stream& stream) const
		{
			stream << "[" << lower() << ":" << upper() << "]";
		}

		constexpr Range<T> toRange() const
			requires std::integral<T>
		{
			return Range<T>{
				.begin = lower(),
				.len = len() + 1,
			};
		}

		static constexpr Segment<T> MakeFromRange(Range<T> const& r)
			requires std::integral<T>
		{
			return Segment<T>(r.begin, r.end() - 1);
		}

		constexpr bool emptyStrict() const
		{
			return !isSortedStrict();
		}

		constexpr bool emptyRelaxed() const
		{
			return !isSortedRelaxed();
		}

		constexpr bool empty(bool strict = true) const
		{
			return strict ? emptyStrict() : emptyRelaxed();
		}

		constexpr Segment pureEmptyChecked(bool strict = true) const
		{
			return empty(strict) ? Segment(EmptyTag{}) : *this;
		}

		constexpr Segment& checkPureEmpty(bool strict = true)
		{
			if (empty(strict))
			{
				*this = Segment(EmptyTag{});
			}
			return *this;
		}
	};
}

template <class Stream, that::impl::SegementScalar S>
Stream& operator<<(Stream& stream, that::Segment<S> const& segment)
{
	segment.printTo(stream);
	return stream;
}