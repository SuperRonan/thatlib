#pragma once

#include <filesystem>
#include <type_traits>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <exception>
#include <vector>
#include <cassert>

#include "Image.h"
#include "Color.h"
#include "../math/Vector.h"

namespace img
{
	namespace io
	{
		using byte = int8_t;
		using pbyte = byte*;
		
		namespace netpbm
		{
			__forceinline bool isNetpbm(std::wstring const& ext)
			{
				return ext == L".ppm" || ext == L".pgm" || ext == L".pbm";
			}

		}

		namespace stbi
		{
			__forceinline bool canReadWrite(std::wstring const& ext)
			{
				return ext == L".png" || ext == L".jpg";
			}
		}

		using RGBu = RGB<unsigned char>;
		using RGBAu = RGBA<unsigned char>;

		template <class T>
		constexpr T TypeMax()noexcept
		{
			if constexpr (std::is_floating_point<T>::value) // Should "is_real", to include other representations (fixed)
			{
				return T(1);
			}
			else if constexpr (std::is_arithmetic<T>::value)
			{
				if constexpr (std::is_same<uint8_t, T>::value)
					return T(255);
			}
			else if constexpr (math::Is_Vector<T>::value)
			{
				if constexpr (std::is_floating_point<T::_Type>::value)
					return T(1);
				else
					return T(255);
			}
			// Should Static warning that the type was not found.
			return T(1);
		}

		__forceinline std::string convertWString(std::wstring const& wstr)
		{
			return std::string(wstr.begin(), wstr.end());
		}
	}
}