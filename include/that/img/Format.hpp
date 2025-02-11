#pragma once

#include <cstdint>
#include <that/math/Vector.hpp>
#include <that/core/BasicTypes.hpp>

namespace that
{
	enum class ElementType : uint16_t
	{
		UNORM, // [0, 1]
		SNORM, // [-1, 1]
		UINT,
		SINT,
		sRGB, // [0, 1]
		FLOAT,
		MAX_ENUM,
	};

	template <class Scalar>
	static constexpr ElementType ElementTypeFromScalar()
	{
		// We miss UNORM / SNORM / sRGB formats...
		if constexpr (std::is_integral<Scalar>::value)
		{
			if constexpr (std::is_unsigned<Scalar>::value)
			{
				return ElementType::UINT;
			}
			else
			{
				return ElementType::SINT;
			}
		}
		if (std::is_floating_point<Scalar>::value)
		{
			return ElementType::FLOAT;
		}
		return ElementType::MAX_ENUM;
	}

	struct FormatInfo
	{
		ElementType type;
		uint8_t elem_size;
		uint8_t channels;

		const uint32_t pixelSize()const
		{
			return elem_size * channels;
		}

		template <class T>
		static constexpr FormatInfo Deduce()
		{
			FormatInfo res;
			if constexpr (math::Is_Vector<T>::value)
			{
				res.channels = T::size();
				using Scalar = typename T::_Type;
				res.type = ElementTypeFromScalar<Scalar>();
				res.elem_size = sizeof(Scalar);
			}
			else
			{
				using Scalar = T;
				res.type = ElementTypeFromScalar<Scalar>();
				res.elem_size = sizeof(Scalar);
				res.channels = 1;
			}
			return res;
		}

		constexpr bool operator==(FormatInfo const& other) const
		{
			return type == other.type && elem_size == other.elem_size && channels == other.channels;
		}
	};

	template <ElementType type, uint32_t size>
	struct UnderlyingPixelType : public std::type_identity<void> {};

	template <uint32_t size>
	struct UnderlyingPixelType<ElementType::UNORM, size> : public UIntTypePerSize<size> {};

	template <uint32_t size>
	struct UnderlyingPixelType<ElementType::SNORM, size> : public IntTypePerSize<size> {};

	template <uint32_t size>
	struct UnderlyingPixelType<ElementType::UINT, size> : public UIntTypePerSize<size> {};

	template <uint32_t size>
	struct UnderlyingPixelType<ElementType::SINT, size> : public IntTypePerSize<size> {};

	template <uint32_t size>
	struct UnderlyingPixelType<ElementType::sRGB, size> : public UIntTypePerSize<size> {};

	template <uint32_t size>
	struct UnderlyingPixelType<ElementType::FLOAT, size> : public FloatTypePerSize<size> {};

	static const char* GetElementTypeName(ElementType t);
}

template <class Stream>
Stream& operator<<(Stream& stream, that::ElementType t)
{
	const char * name = that::GetElementTypeName(t);
	if (name)
	{
		stream << name;
	}
	else
	{
		stream << "UNKNOWN";
	}
	return stream;
}

template <class Stream>
Stream& operator<<(Stream& stream, that::FormatInfo const& fmt)
{
	stream << "Format(" << fmt.type << "_" << fmt.elem_size << " x " << fmt.channels << ")";
	return stream;
}
