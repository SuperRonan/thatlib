#pragma once

#include <that/utils/ScalarTraits.hpp>

#include <that/math/Half.hpp>

#include <bit>

namespace that
{
	template <std::arithmetic Scalar>
	class ScalarTraitOf final : public ScalarTraits
	{
	public:

		Scalar& as(void* scalar_data) const
		{
			return *reinterpret_cast<Scalar*>(scalar_data);
		}

		const Scalar& as(const void* scalar_data) const
		{
			return *reinterpret_cast<Scalar*>(scalar_data);
		}

		template <std::arithmetic Dst>
		Dst to(const void* data)
		{
			return static_cast<Dst>(as(data));
		}

		template <std::arithmetic Src>
		void from(void* data, Src src)
		{
			as(data) = static_cast<Scalar>(src);
		}

		virtual float32_t toF32(const void* scalar) const override final
		{
			return to<float32_t>(scalar);
		}
		virtual float64_t toF64(const void* scalar) const override final
		{
			return to<float64_t>(scalar);
		}
		virtual int32_t   toI32(const void* scalar) const override final
		{
			return to<int32_t>(scalar);
		}
		virtual uint32_t  toU32(const void* scalar) const override final
		{
			return to<uint32_t>(scalar);
		}
		virtual int64_t   toI64(const void* scalar) const override final
		{
			return to<int64_t>(scalar);
		}
		virtual uint64_t  toU64(const void* scalar) const override final
		{
			return to<uint64_t>(scalar);
		}

		virtual void init(void* scalar, float32_t v) const override final
		{
			from(scalar, v);
		}
		virtual void init(void* scalar, float64_t v) const override final
		{
			from(scalar, v);
		}
		virtual void init(void* scalar, int32_t v) const override final
		{
			from(scalar, v);
		}
		virtual void init(void* scalar, uint32_t v) const override final
		{
			from(scalar, v);
		}
		virtual void init(void* scalar, int64_t v) const override final
		{
			from(scalar, v);
		}
		virtual void init(void* scalar, uint64_t v) const override final
		{
			from(scalar, v);
		}

		// scalar and dst must not alias
		virtual void convertToScalar(const void* scalar, ScalarType dst_type, void* dst) const override final
		{
			switch (dst_type)
			{
			case ScalarType::Int8: *reinterpret_cast<i8*>(dst) = to<i8>(scalar); break;
			case ScalarType::UInt8: *reinterpret_cast<u8*>(dst) = to<u8>(scalar); break;
			case ScalarType::Int16: *reinterpret_cast<i16*>(dst) = to<i16>(scalar); break;
			case ScalarType::UInt16: *reinterpret_cast<u16*>(dst) = to<u16>(scalar); break;
			case ScalarType::Int32: *reinterpret_cast<i32*>(dst) = to<i32>(scalar); break;
			case ScalarType::UInt32: *reinterpret_cast<u32*>(dst) = to<u32>(scalar); break;
			case ScalarType::Int64: *reinterpret_cast<i64*>(dst) = to<i64>(scalar); break;
			case ScalarType::UInt64: *reinterpret_cast<u64*>(dst) = to<u64>(scalar); break;
			case ScalarType::Float16: *reinterpret_cast<f16*>(dst) = to<f16>(scalar); break;
			case ScalarType::Float32: *reinterpret_cast<f32*>(dst) = to<f32>(scalar); break;
			case ScalarType::Float64: *reinterpret_cast<f64*>(dst) = to<f64>(scalar); break;
			}
		}

		virtual void add(void* res, const void* lhs, const void* rhs) const override final
		{
			Scalar& r = *static_cast<Scalar*>(res);
			r = as(lhs) + as(rhs);
		}
		virtual void sub(void* res, const void* lhs, const void* rhs) const override final
		{
			Scalar& r = *static_cast<Scalar*>(res);
			r = as(lhs) - as(rhs);
		}
		virtual void mul(void* res, const void* lhs, const void* rhs) const override final
		{
			Scalar& r = *static_cast<Scalar*>(res);
			r = as(lhs) * as(rhs);
		}

		virtual std::partial_ordering compare(const void* lhs, const void* rhs) const override final
		{
			return as(lhs) <=> as(rhs);
		}

		virtual ScalarType getType() const override final
		{
			constexpr size_t type_size_index = std::countr_zero(sizeof(Scalar));
			if constexpr (that::concepts::FloatingPoint<Scalar>)
			{
				return static_cast<ScalarType>(static_cast<size_t>(ScalarType::Float16) + type_size_index - 1);
			}
			else
			{
				constexpr bool s = std::signed_integral<Scalar>;
				return static_cast<ScalarType>(static_cast<size_t>(ScalarType::UInt8) + 2 * type_size_index + (s ? 1 : 0));
			}
		}
	};
}