#pragma once

#include <string>
#include <string_view>
#include <type_traits>
#include <concepts>

namespace that
{
	namespace concepts
	{
		template <class T>
		concept StringLike = std::convertible_to<T, std::string_view>;

		template <class T>
		concept wStringLike = std::convertible_to<T, std::wstring_view>;
	}
}