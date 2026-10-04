#pragma once

#include <concepts>
#include <type_traits>
#include <numeric>
#include <that/core/BasicTypes.hpp>

namespace that
{
	namespace impl
	{
		template <class I, bool AllowSigned>
		concept IntegralAllowSigned = std::integral<I> && (AllowSigned || std::is_unsigned<I>::value);
	}

	// Numerator may be signed
	// Denominator is always unsigned
	// The denominator may be be in the range (uint_max / 2, uint_max) (e.g "sign bit" set to 1), the fraction is valid,
	// vut most operations with it may overflow...
	// If the numeator is int_min (e.g. just the sign bit, 0b100...00)
	// If Numerator is signed, beware if the denominator overflows when converted to signed 
	// Not automatically reduced to canonical form, the user manualy calls canonical().
	template <std::integral Integral>
	struct Rational
	{
		static constexpr const bool IsSigned = std::is_signed<Integral>::value;
		using Numerator = Integral;
		using Denominator = typename std::make_unsigned<Integral>::type;
		using AsSigned = Rational<typename std::make_signed<Numerator>::type>;
		using AsUnsigned = Rational<Denominator>;

		template <std::integral OtherInt>
		using ProdRet_t = std::conditional_t<std::is_signed_v<OtherInt> || IsSigned, AsSigned, Rational>;

		Numerator numerator;
		Denominator denominator;


		// Result of Euclidean division
		struct Euclidean
		{
			Numerator quotient = {};
			Numerator remainder = {};

			constexpr Euclidean operator-() const
			{
				return Euclidean{
					.quotient = -quotient,
					.remainder = -remainder,
				};
			}
		};
		
		constexpr Rational(Numerator n, Denominator d = Denominator(1)):
			numerator(n),
			denominator(d)
		{}

		constexpr Rational(Rational const&) = default;
		constexpr Rational& operator=(Rational const&) = default;

		template <std::integral OtherInt>
		explicit(IntegralConversionExplicit<Numerator, OtherInt>::value)
		constexpr Rational(Rational<OtherInt> const& o) :
			numerator(static_cast<Numerator>(o.numerator)),
			denominator(static_cast<Denominator>(o.denominator))
		{}

		template <std::integral OtherInt>
		constexpr Rational<OtherInt> cast() const
		{
			return Rational<OtherInt>(*this);
		}

		template <std::integral OtherInt>
		explicit(IntegralConversionExplicit<OtherInt, Numerator>::value)
		constexpr operator Rational<OtherInt>() const
		{
			return cast<OtherInt>();
		}

		constexpr Rational<WiderIntIFP_t<Integral>> wider() const
		{
			return cast<WiderIntIFP_t<Integral>>();
		}

		constexpr Rational<NarrowerIntIFP_t<Integral>> narrower() const
		{
			return cast<NarrowerIntIFP_t<Integral>>();
		}

		// Should not be UB because neg value casted to unsigned type
		constexpr Denominator absNumerator() const
		{
			if constexpr (IsSigned)
			{
				if (numerator < 0)  return static_cast<Denominator>(-numerator);
			}
			return static_cast<Denominator>(numerator);
		}

		constexpr Denominator gcd() const
		{
			return std::gcd(numerator, denominator);
		}

		constexpr auto lcm() const
		{
			return std::lcm(numerator, denominator);
		}

		constexpr Rational canonical() const
		{
			const Denominator _gcd = gcd();
			return Rational(numerator / static_cast<Numerator>(_gcd), denominator / _gcd);
		}

		constexpr Rational reduce() const
		{
			return canonical();
		}

		constexpr bool isCanonical() const
		{
			return gcd() == 1;
		}

		constexpr bool valid() const
		{
			return denominator != 0;
		}

		constexpr AsSigned operator-() const
		{
			AsSigned res(-numerator, denominator);
			return res;
		}

		constexpr Rational const& operator+() const
		{
			return *this;
		}

		constexpr Rational abs() const
		{
			return Rational(static_cast<Numerator>(absNumerator()), denominator);
		}

		constexpr AsUnsigned absT() const
		{
			return AsUnsigned(absNumerator(), denominator);
		}

		constexpr Rational inv() const
		{
			if constexpr (IsSigned)
			{
				if (numerator < 0)
				{
					return Rational(-static_cast<Numerator>(denominator), static_cast<Denominator>(-numerator));
				}
			}
			return Rational(denominator, numerator);
		}

		constexpr Rational rcp() const
		{
			return inv();
		}

		template <impl::IntegralAllowSigned<IsSigned> OtherInt>
		constexpr Rational& operator*=(OtherInt rhs)
		{
			numerator *= rhs;
			return *this;
		}

		template <std::integral OtherInt>
		constexpr ProdRet_t<OtherInt> operator*(OtherInt rhs) const
		{
			using Res_t = ProdRet_t<OtherInt>;
			return Res_t(static_cast<typename Res_t::Numerator>(numerator) * rhs, denominator);
		}

		template <impl::IntegralAllowSigned<IsSigned> OtherInt>
		constexpr Rational& operator*=(Rational<OtherInt> const& rhs)
		{
			numerator *= rhs.numerator;
			denominator *= rhs.denominator;
			return *this;
		}

		template <std::integral OtherInt>
		constexpr ProdRet_t<OtherInt> operator*(Rational<OtherInt> const& rhs) const
		{
			using Res_t = ProdRet_t<OtherInt>;
			return Res_t(static_cast<typename Res_t::Numerator>(numerator) * static_cast<typename Res_t::Numerator>(rhs.numerator), denominator * rhs.denominator);
		}

		template <impl::IntegralAllowSigned<IsSigned> OtherInt>
		constexpr Rational& operator/=(OtherInt rhs)
		{
			if constexpr (std::is_signed<OtherInt>::value)
			{
				if (rhs < 0)
				{
					rhs = -rhs;
					numerator = -numerator;
				}
			}
			denominator *= rhs;
			return *this;
		}

		template <std::integral OtherInt>
		constexpr ProdRet_t<OtherInt> operator/(OtherInt rhs) const
		{
			using Res_t = ProdRet_t<OtherInt>;
			Res_t res = Res_t(*this);
			res /= rhs;
			return res;
		}

		template <impl::IntegralAllowSigned<IsSigned> OtherInt>
		constexpr Rational& operator/=(Rational<OtherInt> rhs)
		{
			if constexpr (std::is_signed<OtherInt>::value)
			{
				if (rhs.numerator < 0)
				{
					// I think this adds no extra UB (aside from int * int overflowing)
					numerator = -(numerator * static_cast<Numerator>(rhs.denominator));
					denominator *= Denominator(-rhs.numerator);
					return *this;
				}
			}
			numerator *= rhs.denominator;
			denominator *= rhs.numerator;
			return *this;
		}

		template <std::integral OtherInt>
		constexpr ProdRet_t<OtherInt> operator/(Rational<OtherInt> rhs) const
		{
			using Res_t = ProdRet_t<OtherInt>;
			Res_t res = Res_t(*this);
			res /= rhs;
			return res;
		}

#define DEFINE_RATIONAL_BASIC_OPERATOR(Op) \
		template <std::integral OtherInt> \
		constexpr Rational& operator##Op##=(OtherInt rhs) \
		{ \
			numerator Op##= (rhs * denominator); \
			return *this; \
		} \
		template <std::integral OtherInt> \
		constexpr Rational operator##Op(OtherInt rhs) const \
		{ \
			return Rational(numerator Op rhs * denominator, denominator);\
		} \
		template <std::integral OtherInt> \
		constexpr Rational& operator##Op##=(Rational<OtherInt> const& rhs) \
		{ \
			numerator = numerator * rhs.denominator Op rhs.numerator * denominator;\
			denominator *= rhs.denominator; \
		} \
		template <std::integral OtherInt> \
			constexpr Rational operator##Op(Rational<OtherInt> const& rhs) const\
		{ \
			return Rational(numerator * rhs.denominator Op rhs.numerator * denominator, denominator * rhs.denominator); \
		}

		DEFINE_RATIONAL_BASIC_OPERATOR(+)
		DEFINE_RATIONAL_BASIC_OPERATOR(-)
#undef DEFINE_RATIONAL_BASIC_OPERATOR

		template <std::integral OtherInt>
		constexpr bool equalsBitwise(Rational<OtherInt> const& rhs) const
		{
			return numerator == rhs.numerator && denominator == rhs.denominator;
		}

		template <std::integral OtherInt>
		constexpr std::strong_ordering compareBitwise(Rational<OtherInt> const& rhs) const
		{
			std::strong_ordering res = numerator <=> rhs.numerator;
			if (res == std::strong_ordering::equal)
			{
				res = denominator <=> denominator;
			}
			return res;
		}

		// Beware of the risk of overflow!
		template <std::integral OtherInt>
		constexpr std::strong_ordering compareArithmetic(Rational<OtherInt> const& rhs) const
		{
			return (numerator * rhs.denominator) <=> (denominator * rhs.numerator);
		}

		// Beware of the risk of overflow!
		template <std::integral OtherInt>
		constexpr bool equalsArithmetic(Rational<OtherInt> const& rhs) const
		{
			return compareArithmetic(rhs) == std::strong_ordering::equal;
		}

		// No default operator== is provided

		constexpr Euclidean euclidean() const
		{
			const Numerator d = static_cast<Numerator>(denominator);
			return Euclidean{
				.quotient = static_cast<Numerator>(numerator / d),
				.remainder = static_cast<Numerator>(numerator % d),
			};
		}

		constexpr Numerator quotient() const
		{
			return numerator / static_cast<Numerator>(denominator);
		}

		constexpr Numerator remainder() const
		{
			return numerator % static_cast<Numerator>(denominator);
		}

		template <that::concepts::FloatingPoint F>
		explicit constexpr operator F() const
		{
			return static_cast<F>(numerator) / static_cast<F>(denominator);
		}

		explicit constexpr operator Numerator() const
		{
			return quotient();
		}

		template <class Stream>
		void printTo(Stream& stream) const
		{
			stream << numerator << "/" << denominator;
		}
	};
}

namespace that
{
	template <std::integral I>
	struct SignedType<that::Rational<I>> : std::type_identity<that::Rational<typename std::make_signed<I>::type>> {};
}

template <class Stream, std::integral I>
Stream& operator<<(Stream& stream, that::Rational<I> const& r)
{
	r.printTo(stream);
	return stream;
}