#include <that/utils/Rational.hpp>

namespace that
{
	void test_ratio()
	{
		using Ri = Rational<int>;
		using Ru = Rational<uint>;

		constexpr Ri a(-1, 2);

		static_assert(a.absNumerator() == 1);
		constexpr Ru b = a.absT();

		std::common_type<int, size_t>::type;

		{
			constexpr auto t = -a.denominator;
		}

		{
			constexpr auto eq = int(-9) / int(3);
			constexpr auto er = int(-10) % int(3);
		}

		constexpr Ri c = {5, 8};
		constexpr Ri d = {-9, 6};
		constexpr auto d_gcd = d.gcd();
		constexpr Ri d_c = d.canonical();
		constexpr Ri cd = (c * d).canonical();
		constexpr Ri dc = (c / d).canonical();
		constexpr Ri s = (cd + dc).canonical();
		constexpr float sf = static_cast<float>(s);
		constexpr auto d_e = d.euclidean();

		{
			using Ri8 = Rational<i8>;
			using Ru8 = Rational<u8>;
			using Ri64 = Rational<i64>;
			using Ru64 = Rational<u64>;

			{
				constexpr Ri8 m128(-128, 6);
				constexpr Ru8 p128 = m128.absT();
				constexpr Ri8 p128_ = m128.abs();
				constexpr Ri8 i128 = m128.inv();
				constexpr Ri8 cm128 = m128.canonical();
			}
			{
				constexpr Ri8 o(-6, 240);
				constexpr Ri8 oc = o.canonical();
				constexpr auto o_e = o.euclidean();
			}
			{
				constexpr Ri o(-451'846'683, (1 << 31) | 451'846);
				constexpr Ri64 o_64 = o.cast<i64>();
				constexpr Ri oc = o.canonical();
				constexpr Ri64 o_64c = o_64.canonical();
				constexpr auto o_e = o.euclidean();
				constexpr auto o64_e = o_64.euclidean();
				static_assert(o_e.quotient == o64_e.quotient);
				static_assert(o_e.remainder == o64_e.remainder);
			}
			{
				constexpr Ri8 a(5, 4);
				constexpr Ri8 b(9, 47);
				constexpr Ri8 s = a + b;
				constexpr Rational<i16> s2 = a.wider() + b.wider();
			}
		}
	}
}