#include <that/utils/ScalarTraits.hpp>
#include <that/utils/ScalarTraitsImpl.hpp>

namespace that
{

	void test(float a, float b, int x, int y)
	{
		auto s = x <=> y;
		auto p = a <=> b;
	}
}