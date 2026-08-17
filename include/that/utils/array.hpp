#pragma once

#include <array>

namespace that
{
	template <size_t N, class T>
	static constexpr std::array<T, N> MakeUniformArray(T const& v)
	{
		std::array<T, N> res;
		for(size_t i = 0; i < N; ++i)
		{
			res[i] = v;
		}
		return res;
	}
}