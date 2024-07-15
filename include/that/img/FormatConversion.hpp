#pragma once

#include <bit>

#include <that/core/Core.hpp>

#include <that/img/Format.hpp>

namespace that
{
	template <std::floating_point DstFloat, std::floating_point SrcFloat>
	constexpr DstFloat ConvertFloatToFloat(SrcFloat src)
	{
		const DstFloat res = static_cast<DstFloat>(src);
		return res;
	}

	template <std::floating_point DstFloat, std::integral SrcNorm>
	constexpr DstFloat ConvertNormToFloat(SrcNorm n)
	{
		const SrcNorm max = std::numeric_limits<SrcNorm>::max();
		// As defined by the glsl spec: https://registry.khronos.org/OpenGL-Refpages/gl4/html/unpackUnorm.xhtml
		{
			double d = n;
			if constexpr (std::is_unsigned<SrcNorm>::value)
			{
				d = d / double(max);
			}
			else
			{
				d = std::max<double>(d / double(max), -1.0);
			}
			return ConvertFloatToFloat<DstFloat>(d);
		}
	}

	template <std::integral DstNorm, std::floating_point SrcFloat>
	constexpr DstNorm ConvertFloatToNorm(SrcFloat f)
	{
		double d = static_cast<double>(f);
		const DstNorm max = std::numeric_limits<DstNorm>::max();
		// As defined by the glsl spec: https://registry.khronos.org/OpenGL-Refpages/gl4/html/packUnorm.xhtml
		if constexpr (std::is_unsigned<DstNorm>::value)
		{
			d = std::clamp<double>(d, 0, 1);
		}
		else
		{
			d = std::clamp<double>(d, -1, 1);
		}
		DstNorm res = DstNorm(d * double(max));
		return res;
	}

	template <std::unsigned_integral Target, std::unsigned_integral Uint>
	constexpr Target DuplicateBitsLeft(Uint u)
	{
		if constexpr (sizeof(Target) == sizeof(Uint))
		{
			return u;
		}
		else
		{
			using T = typename UIntTypePerSize<sizeof(Uint) * 2>::type;
			const T tu = static_cast<T>(u);
			T t = tu << sizeof(Uint) | tu;
			return DuplicateBitsLeft<Target>(tu);
		}
	}

	// SNORM -> UNORM: negatives are clamped to 0
	template <std::integral DstNorm, std::integral SrcNorm>
	constexpr DstNorm ConvertNormToNorm(SrcNorm n)
	{
		DstNorm res;
		constexpr const bool src_is_uint = std::is_unsigned<SrcNorm>::value;
		constexpr const bool dst_is_uint = std::is_unsigned<DstNorm>::value;
		using UnsignedSrcNorm = typename UIntTypePerSize<sizeof(SrcNorm)>::type;
		using UnsignedDstNorm = typename UIntTypePerSize<sizeof(DstNorm)>::type;
		if constexpr (sizeof(SrcNorm) == sizeof(DstNorm))
		{
			if constexpr (src_is_uint == dst_is_uint)
			{
				res = n;
			}
			// Avoid overflow
			else if constexpr (dst_is_uint)
			{
				// int -> uint
				SrcNorm clamped = std::max<SrcNorm>(0, n);
				res = std::bit_cast<DstNorm>(clamped) >> 1;
				// Add one unit to the upper half of uint range, so that SRC's 1 is mapped to DST's 1
				if (res & (1 << (sizeof(DstNorm) - 1)))	++res;
			}
			else if constexpr (src_is_uint)
			{
				// uint -> int
				res = std::bit_cast<DstNorm, SrcNorm>(n >> 1);
			}
		}
		if constexpr (sizeof(SrcNorm) > sizeof(DstNorm))
		{
			// Narrowing conversion
			constexpr const SrcNorm mask = ((1 << sizeof(DstNorm)) - 1);
			UnsignedSrcNorm un = std::bit_cast<UnsignedSrcNorm>(n);
			if constexpr (src_is_uint == dst_is_uint)
			{
				un = (un >> (sizeof(SrcNorm) - sizeof(DstNorm)));// & mask; No need for mask, uint shift right is zero filled
				res = std::bit_cast<DstNorm, UnsignedDstNorm>(un);
			}
			else if constexpr (src_is_uint && !dst_is_uint)
			{
				// uint -> int
				// example: u32 -> i8: actually u32 -> i7
				n = (n >> (sizeof(SrcNorm) - sizeof(DstNorm) + 1));// & mask; No need for mask, uint shift right is zero filled & mask;
				res = std::bit_cast<DstNorm, UnsignedDstNorm>(n);
			}
			else if constexpr (!src_is_uint && dst_is_uint)
			{
				// int -> uint
				// example: i32 -> u8: not a simple as i32 -> i8 -> u8 (loses one bit)
				if (n < 0)
				{
					res = 0;
				}
				else
				{
					un = (un >> (sizeof(SrcNorm) - sizeof(DstNorm) - 1));// & mask; No need for mask, uint shift right is zero filled & mask;
					res = std::bit_cast<DstNorm, DstNorm>(un);
				}
			}
		}
		if constexpr (sizeof(SrcNorm) < sizeof(DstNorm))
		{
			// Widening conversion
			// example problem: u8 -> u16: we want 1.0 to be mapped to 1.0
			// so 0xFF -> 0xFFFF (not just 0xFF00)
			// We duplicate the byte
			// 0x00 -> 0x0000 
			// 0x10 -> 0x1010 
			// 0x80 -> 0x8080

			// For signed conversion, we also have to consider the sign bit
			// i8 -> i16
			// 0xF0 -> 0xF000
			// Duplicate without the sign bit
			// But we want:
			// 0x7F -> 0x7FFF, not 0x7F7F
			if constexpr (src_is_uint && dst_is_uint)
			{
				res = DuplicateBitsLeft<DstNorm>(n);
			}
			else
			{
				UnsignedSrcNorm un = std::bit_cast<UnsignedSrcNorm>(n);
				// TODO better
				const double d = ConvertNormToFloat<double>(n);
				res = ConvertFloatToNorm<DstNorm>(d);
			}
		}
		return res;
	}

	template <std::floating_point DstFloat, std::unsigned_integral sRGB>
	constexpr DstFloat ConvertsRGBToFloat(sRGB s)
	{
		using ComputeFloat = typename FloatTypePerSize<std::max<size_t>(4, sizeof(DstFloat))>::type;
		ComputeFloat res = ConvertNormToFloat<ComputeFloat>(s);
		res = std::pow(res, 1.0 / 2.2);
		return res;
	}

	template <std::unsigned_integral sRGB, std::floating_point SrcFloat>
	constexpr sRGB ConvertFloatTosRGB(SrcFloat f)
	{
		using ComputeFloat = typename FloatTypePerSize<std::max<size_t>(4, sizeof(SrcFloat))>::type;
		ComputeFloat d = std::clamp<ComputeFloat>(f, 0, 1);
		f = std::pow(d, 2.2);
		sRGB res = ConvertFloatToNorm<sRGB>(f);
		return res;
	}

	template <std::integral DstNorm, std::unsigned_integral sRGB>
	constexpr DstNorm ConvertsRGBToNorm(sRGB s)
	{
		double d = ConvertsRGBToFloat<double>(s);
		return ConvertFloatToNorm<DstNorm>(d);
	}

	template <std::unsigned_integral sRGB, std::integral SrcNorm>
	constexpr sRGB ConvertNormTosRGB(SrcNorm n)
	{
		double d = ConvertNormToFloat<double>(n);
		return ConvertFloatTosRGB<sRGB>(d);
	}

	template <std::unsigned_integral Dst, std::unsigned_integral Src>
	constexpr Dst ConvertsRGBTosRGB(Src s)
	{
		if constexpr (sizeof(Src) == sizeof(Dst))
		{
			return static_cast<Dst>(s);
		}
		else
		{
			double d = ConvertsRGBToFloat<double>(s);
			Dst res = ConvertFloatTosRGB<Dst>(d);
			return res;
		}
	}

	template <std::integral DstInt, std::floating_point SrcFloat>
	constexpr DstInt ConvertFloatToInt(SrcFloat f)
	{
		return static_cast<DstInt>(f);
	}

	template <std::floating_point DstFloat, std::integral SrcInt>
	constexpr DstFloat ConvertIntToFloat(SrcInt i)
	{
		return static_cast<DstFloat>(i);
	}

	template <ElementType src_type, uint32_t src_size, ElementType dst_type, uint32_t dst_size>
	auto GetConvertPixelChannelFunction()
	{
		using SrcType = typename UnderlyingPixelType<src_type, src_size>::type;
		using DstType = typename UnderlyingPixelType<dst_type, dst_size>::type;
		static_assert(!std::is_same<SrcType, void>::value);
		static_assert(!std::is_same<DstType, void>::value);
		constexpr const auto type_is_int = [](ElementType type) {return type == ElementType::SINT || type == ElementType::UINT; };
		constexpr const auto type_is_norm = [](ElementType type) {return type == ElementType::SNORM || type == ElementType::UNORM; };
		constexpr const auto type_is_srgb = [](ElementType type) {return type == ElementType::sRGB; };
		constexpr const auto type_is_float = [](ElementType type) {return type == ElementType::FLOAT; };

		if constexpr (type_is_int(src_type))
		{
			return [](SrcType const& a, DstType& b) {b = static_cast<DstType>(a); };
		}
		else if constexpr (type_is_norm(src_type))
		{
			if constexpr (type_is_int(dst_type))
			{
				return [](SrcType const& a, DstType& b) {b = static_cast<DstType>(a); };
			}
			else if constexpr (type_is_norm(dst_type))
			{
				return [](SrcType const& a, DstType& b) {b = ConvertNormToNorm<DstType>(a); };
			}
			else if constexpr (type_is_srgb(dst_type))
			{
				return [](SrcType const& a, DstType& b) {b = ConvertNormTosRGB<DstType>(a); };
			}
			else if constexpr (type_is_float(dst_type))
			{
				return [](SrcType const& a, DstType& b) {b = ConvertNormToFloat<DstType>(a); };
			}
			else
			{
				// Would like to static_assert(false), but it is somehow triggered although it should not be
				return [](SrcType const& a, DstType& b) {};
			}
		}
		else if constexpr (type_is_srgb(src_type))
		{
			if constexpr (type_is_int(dst_type))
			{
				return [](SrcType const& a, DstType& b) {b = static_cast<DstType>(a); };
			}
			else if constexpr (type_is_norm(dst_type))
			{
				return [](SrcType const& a, DstType& b) {b = ConvertsRGBToNorm<DstType>(a); };
			}
			else if constexpr (type_is_srgb(dst_type))
			{
				return [](SrcType const& a, DstType& b) {b = ConvertNormToNorm<DstType>(a); }; // Should work too, this conversion does not really make sense anyway
			}
			else if constexpr (type_is_float(dst_type))
			{
				return [](SrcType const& a, DstType& b) {b = ConvertsRGBToFloat<DstType>(a); };
			}
			else
			{
				return [](SrcType const& a, DstType& b) {};
			}
		}
		else if constexpr (type_is_float(src_type))
		{
			if constexpr (type_is_int(dst_type))
			{
				return [](SrcType const& a, DstType& b) {b = ConvertFloatToInt<DstType>(a); };
			}
			else if constexpr (type_is_norm(dst_type))
			{
				return [](SrcType const& a, DstType& b) {b = ConvertFloatToNorm<DstType>(a); };
			}
			else if constexpr (type_is_srgb(dst_type))
			{
				return [](SrcType const& a, DstType& b) {b = ConvertFloatTosRGB<DstType>(a); };
			}
			else if constexpr (type_is_float(dst_type))
			{
				return [](SrcType const& a, DstType& b) {b = ConvertFloatToFloat<DstType>(a); };
			}
			else
			{
				return [](SrcType const& a, DstType& b) {};
			}
		}
		else
		{
			return [](SrcType const& a, DstType& b) {};
		}
	}
}