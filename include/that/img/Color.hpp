#pragma once

#include <that/math/Vector.hpp>
#include <type_traits>
#include <limits>

namespace that
{
	namespace img
	{
		template <class T> 
		constexpr T One()
		{
			if constexpr (std::is_floating_point<T>::value)
				return T(1);
			if constexpr (std::is_integral<T>::value)
				return std::numeric_limits<T>::max();
		
			return T(1);
		}

		template <int N, class T>
		using Spectrum = math::Vector<N, T>;

		template <class T>
		using RGB = Spectrum<3, T>;

		template <class T>
		using Grey = Spectrum<1, T>;

		template <class T>
		struct RGBA
		{
			RGB<T> rgb;
			T a;

			using _Type = T;

			constexpr RGBA(RGBA const& other) = default;

			constexpr RGBA(T const& r = 0, T const& g = 0, T const& b = 0, T const& a = One<T>()) :
				rgb(r, g, b),
				a(a)
			{}

			constexpr RGBA(RGB<T> const& rgb, T const& a = One<T>()) : 
				rgb(rgb),
				a(a)
			{}

			constexpr RGBA operator=(RGBA const& other)
			{
				rgb = other.rgb;
				a = other.a;
				return *this;
			}

			constexpr RGBA operator=(RGB<T> const& other)
			{
				rgb = other;
				a = One<T>();
				return *this;
			}

			constexpr operator RGB<T>()const
			{
				return rgb;
			}

			constexpr operator Grey<T>()const
			{
				return rgb[0];
			}
		};

		template <class T>
		class is_RGB : public std::false_type {};

		template <class T>
		class is_RGB<RGB<T>> : public std::true_type {};

		template <class T>
		class is_RGBA : public std::false_type {};

		template <class T>
		class is_RGBA<RGBA<T>> : public std::true_type {};

		template <class T>
		class is_Grey : public std::false_type {};

		template <class T>
		class is_Grey<Grey<T>> : public std::true_type {};
	}
}

namespace std
{
	template <class T>
	class is_trivially_copyable<that::img::RGBA<T>> : public is_trivially_copyable<T>::type {};
};