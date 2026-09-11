#pragma once

#include <array>
#include <that/stl_ext/const_forward.hpp>

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

	namespace impl
	{
		template <class OutElem, size_t N, class InputElemType, class It, class F>
		static constexpr std::array<OutElem, N> TransfromArray(It&& it, F&& f)
		{
			std::array<OutElem, N> res;
			// Could use std::transform, but don't want to include it
			for (size_t i = 0; i < N; ++i)
			{
				res[i] = f(std::forward_move(*(it++)));
			}
			return res;
		}
	}

	template <class InputElem, size_t N, class F>
	static constexpr auto TransfromArray(std::array<InputElem, N> const& a, F&& f)
	{
		using OutputElem = std::invoke_result_t<F, InputElem>;
		return impl::TransfromArray<OutputElem, N, InputElem>(a.data(), f);
	}

	template <class InputElem, size_t N, class F>
	static constexpr auto TransfromArray(std::array<InputElem, N>&& a, F&& f)
	{
		using OutputElem = std::invoke_result_t<F, InputElem>;
		return impl::TransfromArray<OutputElem, N, InputElem>(a.data(), f);
	}
}