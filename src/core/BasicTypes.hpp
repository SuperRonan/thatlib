#pragma once

#include <type_traits>
#include <cstdint>

//#include <stdfloat>

namespace that
{
	template <size_t s>
	struct FloatTypePerSize : public std::type_identity<void> {};

	template <>
	struct FloatTypePerSize<sizeof(float)> : public std::type_identity<float> {};

	template <>
	struct FloatTypePerSize<sizeof(double)> : public std::type_identity<double> {};

	template <size_t s>
	struct IntTypePerSize : public std::type_identity<void> {};

	template <>
	struct IntTypePerSize<sizeof(int8_t)> : public std::type_identity<int8_t> {};

	template <>
	struct IntTypePerSize<sizeof(int16_t)> : public std::type_identity<int16_t> {};

	template <>
	struct IntTypePerSize<sizeof(int32_t)> : public std::type_identity<int32_t> {};

	template <>
	struct IntTypePerSize<sizeof(int64_t)> : public std::type_identity<int64_t> {};

	template <size_t s>
	struct UIntTypePerSize : public std::type_identity<void> {};

	template <>
	struct UIntTypePerSize<sizeof(uint8_t)> : public std::type_identity<uint8_t> {};

	template <>
	struct UIntTypePerSize<sizeof(uint16_t)> : public std::type_identity<uint16_t> {};

	template <>
	struct UIntTypePerSize<sizeof(uint32_t)> : public std::type_identity<uint32_t> {};

	template <>
	struct UIntTypePerSize<sizeof(uint64_t)> : public std::type_identity<uint64_t> {};

	template <size_t s>
	using float_st = typename FloatTypePerSize<s>::type;

	template <size_t s>
	using int_st = typename IntTypePerSize<s>::type;

	template <size_t s>
	using sint_st = int_st<s>;

	template <size_t s>
	using uint_st = typename UIntTypePerSize<s>::type;
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

// TODO with C++ 23 fixed float types
using f32 = float;
using f64 = double;

using float32_t = f32;
using float64_t = f64;
