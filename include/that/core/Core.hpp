#pragma once

#define COMMA ,

#define THAT_BREAKPOINT_HANDLE {int _ = 0;}

#define FORWARD_MACRO(X) X

namespace that
{
	template <bool value>
	constexpr void StaticAssert()
	{
		static_assert(value);
	}
}