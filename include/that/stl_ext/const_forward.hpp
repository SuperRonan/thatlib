#pragma once

#include <utility>

namespace std
{
	template <class T>
	constexpr T const& forward(typename std::remove_reference<T>::type const& t)
	{
		return t;
	}
}