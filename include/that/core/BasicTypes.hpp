#pragma once

#include <type_traits>
#include <cstdint>
#include <limits>
#include <concepts>
#include <bit>

#include <that/core/Concepts.hpp>

//#include <stdfloat>

namespace that
{
	template <size_t size_of>
	struct FloatTypePerSize : public std::type_identity<void> {};

	template <>
	struct FloatTypePerSize<sizeof(float)> : public std::type_identity<float> {};

	template <>
	struct FloatTypePerSize<sizeof(double)> : public std::type_identity<double> {};

	template <size_t size_of>
	struct IntTypePerSize : public std::type_identity<void> {};

	template <>
	struct IntTypePerSize<sizeof(int8_t)> : public std::type_identity<int8_t> {};

	template <>
	struct IntTypePerSize<sizeof(int16_t)> : public std::type_identity<int16_t> {};

	template <>
	struct IntTypePerSize<sizeof(int32_t)> : public std::type_identity<int32_t> {};

	template <>
	struct IntTypePerSize<sizeof(int64_t)> : public std::type_identity<int64_t> {};

	template <size_t size_of>
	struct UIntTypePerSize : public std::type_identity<void> {};

	template <>
	struct UIntTypePerSize<sizeof(uint8_t)> : public std::type_identity<uint8_t> {};

	template <>
	struct UIntTypePerSize<sizeof(uint16_t)> : public std::type_identity<uint16_t> {};

	template <>
	struct UIntTypePerSize<sizeof(uint32_t)> : public std::type_identity<uint32_t> {};

	template <>
	struct UIntTypePerSize<sizeof(uint64_t)> : public std::type_identity<uint64_t> {};

	template <std::integral Int, bool Signed>
	using IntWithSign_t = std::conditional<Signed, typename std::make_signed<Int>::type, typename std::make_unsigned<Int>::type>::type;

	template <std::integral Int>
	static constexpr bool CanHoldValue(IntWithSign_t<size_t, std::signed_integral<Int>> Value)
	{
		using SZ = IntWithSign_t<size_t, std::signed_integral<Int>>;
		using limits = std::numeric_limits<Int>;
		return Value >= static_cast<SZ>(limits::min()) && Value <= static_cast<SZ>(limits::max());
	}

	namespace impl
	{
		template<uintmax_t MaxValueIncluded>
		static consteval size_t RequiredBytes()
		{
			// Use ternary to avoid including <algorithms> for std::max
			return MaxValueIncluded == 0 ? 1 : (std::bit_width(MaxValueIncluded) + 7) / 8;
		}

		template<uintmax_t MaxValueIncluded>
		static consteval size_t RequireBytesPo2()
		{
			return std::bit_ceil(impl::RequiredBytes<MaxValueIncluded>());
		}
	}
	template<uintmax_t MaxValueIncluded>
	using BigEnoughUInt = UIntTypePerSize<impl::RequireBytesPo2<MaxValueIncluded>()>;


	template <size_t s>
	using float_st = typename FloatTypePerSize<s>::type;

	template <size_t s>
	using int_st = typename IntTypePerSize<s>::type;

	template <size_t s>
	using sint_st = int_st<s>;

	template <size_t s>
	using uint_st = typename UIntTypePerSize<s>::type;

	namespace impl
	{
		// Extend this one
		template <class T>
		struct IsFloatingPoint : std::is_floating_point<T> {};
	}

	// that::IsFloatingPoint can be extended, (std::is_floating_point cannot)
	// Prefer specializing the impl version
	template <class T>
	using IsFloatingPoint = impl::IsFloatingPoint<typename std::remove_cv<T>::type>;

	namespace concepts
	{
		template <class T>
		concept FloatingPoint = IsFloatingPoint<T>::value;
	}

	template <concepts::FloatingPoint Float>
	struct FloatingPointCompute : std::type_identity<Float> {};

	template <concepts::FloatingPoint Float>
	using FloatingPointComputeType = typename FloatingPointCompute<Float>::Type;
}

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using s8 = i8;
using s16 = i16;
using s32 = i32;
using s64 = i64;

using sint8_t = int8_t;
using sint16_t = int16_t;
using sint32_t = int32_t;
using sint64_t = int64_t;

using uchar = unsigned char;
using schar = signed char;
using ushort = unsigned short;
using sshort = signed short;
using uint = unsigned int;
using sint = signed int;
using ulong = unsigned long;
using slong = signed long;
using ullong = unsigned long long;
using sllong = signed long long;

// TODO with C++ 23 fixed float types (when made available by the compile (hopefully sometime b4 C++ 26 releases))
#if __STDCPP_FLOAT16_T__
using f16 = float16_t;
#else
// include half
#endif
#if __STDCPP_FLOAT32_T__
using f32 = std::float32_t;
#else
using f32 = float;
#endif
#if __STDCPP_FLOAT64_T__
using f64 = std::float64_t;
#else
using f64 = double;
#endif

using float32_t = f32;
using float64_t = f64;


namespace std
{
	// Given a native type, get the signed version of it
	// std::make_signed only works on native integral (and cannot be extended),
	// std::signed_type also works with floats, and eventualy other user defined types
	template <class T>
	struct signed_type;

	template <std::integral I>
	struct signed_type<I> : std::make_signed<I> {};

	template <that::concepts::FloatingPoint F>
	struct signed_type<F> : std::type_identity<F> {};

	template <class T>
	using signed_type_t = typename signed_type<T>::type;
	
	template <class T>
	concept arithmetic = is_arithmetic<T>::value;
}
