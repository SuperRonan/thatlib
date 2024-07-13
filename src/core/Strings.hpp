#pragma once

#include <string>
#include <string_view>
#include <type_traits>
#include <concepts>

namespace that
{
	namespace concepts
	{
		template <class T, class char_t>
		concept BasicStringLike = std::convertible_to<T, std::basic_string_view<char_t>>;

		template <class T>
		concept StringLike = BasicStringLike<T, char>;

		template <class T>
		concept wStringLike = BasicStringLike<T, wchar_t>;
	}

	template <class char_t, concepts::BasicStringLike<char_t> StringType>
	constexpr size_t GetBasicStringSize(StringType const& str)
	{
		using RawStrType = typename std::remove_cvref<StringType>::type;
		if constexpr (std::is_same<RawStrType, const char*>::value || std::is_same<RawStrType, char*>::value)
		{
			return std::strlen(str);
		}
		else if constexpr (std::is_same<RawStrType, const wchar_t*>::value || std::is_same<RawStrType, wchar_t*>::value)
		{
			return std::wcslen(str);
		}
		else
		{
			return str.size();
		}
	}
}