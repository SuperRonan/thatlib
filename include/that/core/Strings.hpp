#pragma once

#include <string>
#include <string_view>
#include <type_traits>
#include <concepts>
#include <that/core/Concepts.hpp>

namespace that
{
	namespace concepts
	{
		template <class T, class char_t>
		concept BasicStringLike = std::convertible_to<T, std::basic_string_view<char_t>>;

		template <class T, class char_t>
		using BasicStringLikePredictor= std::bool_constant<BasicStringLike<T, char_t>>;

		template <class T>
		concept StringLike = BasicStringLike<T, char>;

		template <class T>
		concept wStringLike = BasicStringLike<T, wchar_t>;

#define _THAT_STRING_CHAR_LIST char, wchar_t, char8_t, char16_t, char32_t

		template <class T>
		concept StringChar = AnyOf<typename std::remove_cvref<T>::type, std::is_same, _THAT_STRING_CHAR_LIST>;

		template <class T>
		concept RawStringPtr = std::is_pointer<T>::value && StringChar<typename std::remove_pointer<T>::type>;

		template <class T>
		concept GenericString = AnyOf<T, BasicStringLikePredictor, _THAT_STRING_CHAR_LIST>; 
	}

	template <concepts::GenericString Str>
	using GenericStringCharType = FirstMatch<Str, concepts::BasicStringLikePredictor, _THAT_STRING_CHAR_LIST>::type;

	template <concepts::GenericString Str>
	constexpr size_t GetGenericStringSize(Str const& str)
	{
		//if constexpr (std::is_bounded_array<Str>::value)
		//{
		//	return std::extent<Str>::value - 1;
		//}
		//else
		{
			using char_t = GenericStringCharType<Str>;
			const std::basic_string_view<char_t> sv = str;
			return sv.size();
		}
	}

	template <concepts::GenericString Str, class char_t>
	static void ConvertString(Str const& src, std::basic_string<char_t> & dst)
	{
		dst.resize(GetGenericStringSize(src));
		for (size_t i = 0; i < dst.size(); ++i)
		{
			dst[i] = char_t(src[i]);
		}
	}
}