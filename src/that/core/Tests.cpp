#include <that/core/Strings.hpp>

#include <that/utils/StringMap.hpp>
#include <that/utils/StringSet.hpp>

#include <that/core/BasicTypes.hpp>

#include <that/img/Color.hpp>

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

	void f()
	{
		std::string str;
		static_assert(concepts::BasicStringLike<decltype(str), char> == true);
		GetGenericStringSize(str);

		StringSet<char> set;
		set.insert("abc");

		static_assert(std::same_as<BigEnoughUInt<196>::type, u8>);
		static_assert(std::same_as<BigEnoughUInt<5000>::type, u16>);
		static_assert(std::same_as<BigEnoughUInt<50'000'000>::type, u32>);
		static_assert(std::same_as<BigEnoughUInt<4'000'000'000'000'000>::type, u64>);
	}

	static constexpr const size_t N = GetGenericStringSize("abc");
}