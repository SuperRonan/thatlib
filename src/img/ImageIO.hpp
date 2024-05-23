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
#include <format>

#include "Image.hpp"
#include "Color.hpp"
#include "../math/Vector.hpp"


namespace that
{
	namespace img
	{
		namespace io
		{
			using byte = uint8_t;
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
					return ext == L".png" || ext == L".jpg" || ext == L".jpeg" || ext == L".tga" || ext == L".hdr";
				}
			}

			using RGBu = RGB<unsigned char>;
			using RGBAu = RGBA<unsigned char>;
			using Grey8u = Grey<uint8_t>;

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
					if constexpr (std::is_floating_point<typename T::_Type>::value)
						return T(1);
					else
						return T(255);
				}
				// Should Static warning that the type was not found.
				return T(1);
			}

			inline std::string convertWString(std::wstring const& wstr)
			{
				std::string res;
				res.resize(wstr.size());
				std::transform(wstr.begin(), wstr.end(), res.begin(), [](wchar_t wc) {return static_cast<char>(wc); });
				return res;
				//return std::string(wstr.begin(), wstr.end());
			}
		}
}

}