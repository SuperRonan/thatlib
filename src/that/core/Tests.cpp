#include <that/core/Strings.hpp>

#include <that/utils/StringMap.hpp>
#include <that/utils/StringSet.hpp>

#include <that/core/BasicTypes.hpp>

#include <that/img/Color.hpp>

#include <that/math/Half.hpp>

namespace that
{
	namespace concepts
	{
		static_assert(GenericString<std::string>);
		static_assert(GenericString<std::string_view>);
		static_assert(GenericString<std::wstring>);
		static_assert(GenericString<std::wstring_view>);
		static_assert(GenericString<const char*>);
		static_assert(GenericString<char*>);
		static_assert(GenericString<wchar_t*>);
		static_assert(GenericString<char* const>);
		static_assert(GenericString<const char* const>);

		static_assert(BinaryTypePredictor<std::is_same>);
		static_assert(BinaryTypePredictor<BasicStringLikePredictor>);
		static_assert(UnaryTypePredictor<std::is_pointer>);
	}

	static_assert(std::is_default_constructible<math::Vector<4, int>>::value);
	static_assert(std::is_trivially_copyable<math::Vector<4, int>>::value);
	static_assert(std::is_trivially_destructible<math::Vector<4, int>>::value);

	static_assert(std::is_default_constructible<img::RGBA<float>>::value);
	static_assert(std::is_trivially_copyable<img::RGBA<float>>::value);
	static_assert(std::is_trivially_destructible<img::RGBA<float>>::value);

	template <std::unsigned_integral UInt>
	static consteval bool CheckUIntType()
	{
		using Limits = std::numeric_limits<UInt>;
		constexpr bool res1 = std::same_as<typename BigEnoughUInt<Limits::max()>::type, UInt>;
		static_assert(res1);
		return res1;
	}

	void f()
	{
		std::string str;
		static_assert(concepts::BasicStringLike<decltype(str), char> == true);
		GetGenericStringSize(str);

		StringSet<char> set;
		set.insert("abc");
		constexpr const size_t B0 = impl::RequireBytesPo2<0>();
		constexpr const size_t B1 = impl::RequireBytesPo2<1>();
		constexpr const size_t B4M = impl::RequireBytesPo2<4'000'000>();
		constexpr const size_t B4_15 = impl::RequireBytesPo2<4'000'000'000'000'000>();
		static_assert(std::same_as<BigEnoughUInt<0>::type, u8>);
		static_assert(std::same_as<BigEnoughUInt<1>::type, u8>);
		static_assert(std::same_as<BigEnoughUInt<196>::type, u8>);
		static_assert(std::same_as<BigEnoughUInt<5000>::type, u16>);
		static_assert(std::same_as<BigEnoughUInt<4'000'000>::type, u32>);
		static_assert(std::same_as<BigEnoughUInt<50'000'000>::type, u32>);
		static_assert(std::same_as<BigEnoughUInt<4'000'000'000'000'000>::type, u64>);

		static_assert(CheckUIntType<u8>());
		static_assert(CheckUIntType<u16>());
		static_assert(CheckUIntType<u32>());
		static_assert(CheckUIntType<u64>());

		{
			constexpr f16 a = 12;
			constexpr f16 b = -9;
			constexpr f16 c = a + b;
			constexpr f16 d = a * c;
			constexpr f32 e = c;
			constexpr f32 f = d;
		}
	}

	static constexpr const size_t N = GetGenericStringSize("abc");
	static_assert(N == 3);

	static_assert(sizeof(f32) == 4);
	static_assert(sizeof(f64) == 8);
}