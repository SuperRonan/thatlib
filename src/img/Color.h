#pragma once

#include "../math/Vector.h"

namespace img
{
	template <int N, class T>
	using Spectrum = math::Vector<N, T>;

	template <class T>
	using RGB = Spectrum<3, T>;

}