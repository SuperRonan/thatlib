#pragma once

#include <that/core/BasicTypes.hpp>
#include <that/Math/Half.hpp>
#include <compare>

namespace that
{
	// C++ fundamental aithmetic types
	enum class ScalarType
	{
		UInt8,
		Int8,
		UInt16,
		Int16,
		UInt32,
		Int32,
		UInt64,
		Int64,
		Float16,
		Float32,
		Float64,

		None,
	};

	struct ScalarStorage
	{
		static constinit const size_t SizeOf = sizeof(size_t);
		union
		{
			alignas(SizeOf) char _[SizeOf];
			u8 _u8;
			i8 _i8;
			u16 _u16;
			i16 _i16;
			u32 _u32;
			i32 _i32;
			u64 _u64;
			i64 _i64;
			f16 _f16;
			f32 _f32;
			f64 _f64;
		};

		const void* data() const
		{
			return _;
		}

		void* data()
		{
			return _;
		}
	};

	class ScalarTraits
	{
	public:
		virtual float32_t toF32(const void* scalar) const = 0;
		virtual float64_t toF64(const void* scalar) const = 0;
		virtual int32_t   toI32(const void* scalar) const = 0;
		virtual uint32_t  toU32(const void* scalar) const = 0;
		virtual int64_t   toI64(const void* scalar) const = 0;
		virtual uint64_t  toU64(const void* scalar) const = 0;

		virtual void init(void* scalar, float32_t v) const = 0;
		virtual void init(void* scalar, float64_t v) const = 0;
		virtual void init(void* scalar, int32_t v) const = 0;
		virtual void init(void* scalar, uint32_t v) const = 0;
		virtual void init(void* scalar, int64_t v) const = 0;
		virtual void init(void* scalar, uint64_t v) const = 0;

		// scalar and dst must not alias
		virtual void convertTo(const void* scalar, ScalarType dst_type, void* dst) const = 0;

		virtual void add(void* res, const void* lhs, const void* rhs) const = 0;
		virtual void sub(void* res, const void* lhs, const void* rhs) const = 0;
		virtual void mul(void* res, const void* lhs, const void* rhs) const = 0;

		virtual std::partial_ordering compare(const void* lhs, const void* rhs) const = 0;

		virtual ScalarType getType() const = 0;
	};

	struct ScalarTraitsStorage
	{
		
	};
}