#pragma once

#define COMMA ,

#define THAT_BREAKPOINT_HANDLE {int _ = 0;}

#define FORWARD_MACRO(X) X

#define DECLARE_ENUM_VALUE_1(Name) Name,
#define DECLARE_ENUM_VALUE_2(Name, Value) Name = Value,

#define X_STRINGIFY(a) STRINGIFY(a)
#define STRINGIFY(a) #a

#define DECLARE_STRINGIFIED(X) DECLARE_ENUM_VALUE_1(STRINGIFY(X))
#define DECLARE_STRINGIFIED_2(X, V) DECLARE_ENUM_VALUE_1(STRINGIFY(X))

namespace that
{
	template <bool value>
	constexpr void StaticAssert()
	{
		static_assert(value);
	}
}