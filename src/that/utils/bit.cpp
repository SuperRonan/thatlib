#include <that/utils/bit.hpp>

namespace that
{
	void test_bit()
	{
		static_assert(ShiftLeftUnchecked(0b11u, 1u) == 0b110u);
		static_assert(ShiftLeftUnchecked(0b11u, -1) == 0b1u);
	}
}