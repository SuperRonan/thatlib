#include <that/utils/array.hpp>

#include <string>

void test_array()
{
	std::array a{1, 2, 3};
	std::array labels = that::TransfromArray(a, [](int x){return std::to_string(x); });
}