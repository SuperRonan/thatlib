#pragma once

#define THAT_BREAKPOINT_HANDLE {int _ = 0;}

namespace that
{
	template <bool value>
	constexpr void StaticAssert()
	{
		static_assert(value);
	}
}