#include <that/core/Strings.hpp>

#include <that/utils/StringMap.hpp>
#include <that/utils/StringSet.hpp>

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

	void f()
	{
		std::string str;
		GetGenericStringSize(str);

		StringSet<char> set;
		set.insert("abc");
	}

	static constexpr const size_t N = GetGenericStringSize("abc");
}