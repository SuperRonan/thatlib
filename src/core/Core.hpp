#pragma once



namespace that
{
	template <bool value>
	constexpr static void StaticAssert()
	{
		static_assert(value);
	}
}