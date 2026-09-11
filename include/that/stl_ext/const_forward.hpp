#pragma once

#include <utility>

namespace std
{
	// Removed it, it might break other code + It was a bad idea in the first place
	// Instead, Rely on T const& std::move(T const&);, or std::forward_move to be explicit
	//template <class T>
	//constexpr T const& forward(typename std::remove_reference<T>::type const& t)
	//{
	//	return t;
	//}

	// Explicit forward move
	template <class _Ty>
	constexpr remove_reference_t<_Ty>&& forward_move(_Ty&& _Arg) noexcept
	{
		return std::move(_Arg);
	}
}