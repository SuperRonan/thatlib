#pragma once



namespace that
{
	template <bool value>
	constexpr void StaticAssert()
	{
		static_assert(value);
	}
}