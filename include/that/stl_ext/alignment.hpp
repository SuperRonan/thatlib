#pragma once

#include <concepts>
#include <cassert>
#include <bit>

namespace std
{
	template <integral Uint> 
	constexpr Uint bitMask(Uint n)
	{
		return (Uint(1) << n) - Uint(1);
	}

	// returns true for 0
	template <integral Uint>
	constexpr bool isPowerOf2(Uint i)
	{
		return ((i - 1) & i) == 0;
	}

	template <integral Uint>
	constexpr bool isPo2(Uint i)
	{
		return isPowerOf2(i);
	}

	template <integral Uint>
	constexpr Uint alignUp(Uint n, Uint a)
	{
		const Uint r = n % a;
		return r ? n + (a - r) : n;
	}

	template <integral Uint>
	constexpr Uint alignUpAssumePo2(Uint n, Uint a_p2)
	{
		assert(isPowerOf2(a_p2));
		assert(a_p2 > 1);
		const Uint a_mask = a_p2 - 1;
		const Uint res = (n + a_mask) & ~a_mask;
		assert(res == alignUp(n, a_p2));
		return res;
	}

	template <integral Uint>
	constexpr Uint alignDown(Uint n, Uint a)
	{
		return n - (n % a);
	}

	template <integral Uint>
	constexpr Uint alignDownAssumePo2(Uint n, Uint a_p2)
	{
		assert(isPowerOf2(a_p2));
		assert(a_p2 > 1);
		const Uint a_mask = a_p2 - 1;
		const Uint res = n & ~a_mask;
		assert(res == alignDown(n, a_p2));
		return res;
	}

	// returns p such as:
	// p >= n &&
	// isPowerOf2(p) == true
	template <integral Uint>
	constexpr Uint roundToNextPo2(Uint n)
	{
		Uint res = n;
		if (!isPowerOf2(n))
		{
			res = std::bit_ceil(n);
		}
		return res;
	}

	template <integral Uint>
	constexpr Uint divDown(Uint num, Uint div)
	{
		return num / div;
	}

	template <integral Uint>
	constexpr Uint divUpAssumeNoOverflow(Uint num, Uint div)
	{
		assert(num < (num + (div - 1))); // No overflow
		return (num + (div - 1)) / div;
	}

	template <integral Uint>
	constexpr Uint divUpSafe(Uint num, Uint div)
	{
		// div instruction computes the quotient and remaineder, so it is fast
		Uint res = num / div;
		Uint remainder = num % div;
		if (remainder)	++res;
		return res;
	}
}