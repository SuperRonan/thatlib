#pragma once

#include <cstdint>
#include <cmath>
#include <algorithm>
#include <bit>

#include <that/core/BasicTypes.hpp>

namespace that
{

	namespace math
	{
		// Basic Half precision floating point type. Use for storage, not computations
		// Will be deprecated in C++23 that will add language support for float16_t
		// Could make a generic storage float class with different sizes / exponent mantissa ratio / optional sign bit
		class Half
		{
		public:

			constexpr static uint32_t SignOffset32()
			{
				return ExponentOffset32() + ExponentBits32();
			}

			constexpr static uint32_t ExponentOffset32()
			{
				return MantissaBits32();
			}

			constexpr static uint32_t ExponentBits32()
			{
				return 8;
			}

			constexpr static uint32_t MantissaBits32()
			{
				return 23;
			}

			constexpr static uint16_t SignOffset16()
			{
				return ExponentOffset16() + ExponentBits16(); 
			}

			constexpr static uint16_t ExponentOffset16()
			{
				return MantissaBits16();
			}

			constexpr static uint16_t ExponentBits16()
			{
				return 5;
			}

			constexpr static uint16_t MantissaBits16()
			{
				return 10;
			}

			constexpr static uint32_t SignMask32()
			{
				return uint32_t(1 << SignOffset32());
			}

			constexpr static uint32_t ExponentMask32()
			{
				constexpr const uint32_t res = uint32_t(((1 << ExponentBits32()) - 1) << ExponentOffset32());
				return res;
			}

			constexpr static uint32_t MantissaMask32()
			{
				constexpr const uint32_t res = uint32_t((1 << MantissaBits32()) - 1);
				return res;
			}

			constexpr static uint16_t SignMask16()
			{
				return uint16_t(1 << SignOffset16());
			}

			constexpr static uint16_t ExponentMask16()
			{
				constexpr const uint16_t res = uint16_t(((1 << ExponentBits16()) - 1) << ExponentOffset16());
				return res;
			}

			constexpr static uint16_t MantissaMask16()
			{
				constexpr const uint16_t res = uint16_t((1 << MantissaBits16()) - 1);
				return res;
			}

		protected:

			struct DetailedHalf
			{
				uint16_t mantissa : 10;
				uint16_t exponent : 5;
				uint16_t sign : 1;
			};

			union
			{
				uint16_t _data = 0;
				DetailedHalf _detailed;
			};

		public:

			constexpr Half() noexcept = default;
			constexpr Half(Half const&) noexcept = default;

			constexpr Half& operator=(Half const&) noexcept = default;

			Half(float32_t f) noexcept
			{
				// Fast conversion, no checks for special cases 
				const uint32_t &u = reinterpret_cast<uint32_t&>(f);
				const uint32_t exp32_bin = (u & ExponentMask32()) >> ExponentOffset32();
				const uint32_t mantissa32 = u & MantissaMask32();
				constexpr const uint16_t mantissa_shift = MantissaBits32() - MantissaBits16();
				
				_data = ((u & SignMask32()) >> 16);
				
				if (exp32_bin != 0)
				{
					const int32_t exp32 = int32_t(exp32_bin - 127);
					int16_t exp16 = std::clamp<int32_t>(exp32, -15, 15);
					uint16_t exp16_bin = std::bit_cast<uint16_t, int16_t>(exp16 + int16_t(15));
					uint16_t mantissa16 = uint16_t(mantissa32 >> mantissa_shift) & MantissaMask16();
					if (exp32 <= 15)
					{

					}
					_data |= (exp16_bin << ExponentOffset16());
					_data |= (mantissa16);

					// if remaining mantissa > half its capacity, increment
					if (mantissa32 & (1 << (mantissa_shift - 1)))
					{
						++_data;
					}
				
				}
			}

			operator float32_t() noexcept
			{
				// inspired by https://fgiesen.wordpress.com/2012/03/28/half-to-float-done-quic/
				float32_t res;
				uint32_t & u = reinterpret_cast<uint32_t&>(res);
				u = ((_data & SignMask16()) << 16);
				const uint16_t exp16_bin = ((_data & ExponentMask16()) >> ExponentOffset16());
				const uint16_t mantissa16 = _data & MantissaMask16();
				constexpr const uint16_t mantissa_shift = MantissaBits32() - MantissaBits16();
				uint32_t exp32_bin = 0;
				uint32_t mantissa32 = mantissa16;
				if (exp16_bin == 0)
				{
					if (mantissa16 != 0)
					{
						exp32_bin = 0;
						int e = -1;
						uint32_t m = mantissa16;
						do
						{
							++e;
							m = (m << 1);
						} while((m & 0x400) == 0);
						mantissa32 = (m & 0x3FF);
						exp32_bin = 127 - 15 - e;
					}
				}
				else
				{
					exp32_bin = uint32_t(int(exp16_bin) - 15 + 127);
				}
				
				u |= (exp32_bin << ExponentOffset32());
				u |= (mantissa32 << mantissa_shift);
				
				if (res > 130000)
				{
					int _ = 0;
				}

				return res;
			}
		};

		using float16_t = Half;
	}

	template <>
	struct FloatTypePerSize<sizeof(that::math::Half)> : public std::type_identity<that::math::Half> {};
}

namespace std
{
	// TODO correctly
	// Since the Half type is only for storage, and not for computation, we can get aways with these values not being correct
	template <>
	class numeric_limits<that::math::Half> : public std::numeric_limits<float>
	{
		static constexpr int digits = 11;
		static constexpr int digits10 = 3;
		static constexpr int max_digits10 = 4; //???
		static constexpr int max_exponent = 15;
		static constexpr int max_exponent10 = 4;
		static constexpr int min_exponent = -14;
		static constexpr int min_exponent10 = -4;
	};

	template <>
	inline constexpr bool is_floating_point_v<that::math::Half> = true;

	template<>
	struct is_floating_point<that::math::Half> : public std::true_type {};
}