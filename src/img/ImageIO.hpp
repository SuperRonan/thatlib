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

#include <core/Strings.hpp>

namespace that
{
	namespace io
	{
		extern Result CreateDirectoryCannonicalIFN(std::filesystem::path const& cannonical_path);
		extern Result CreateDirectoryIFN(std::filesystem::path const& path);
	}

	namespace img
	{
		namespace io
		{
			using byte = uint8_t;
			using pbyte = byte*;
		
			namespace netpbm
			{
				extern bool IsNetpbm(std::string_view const& ext);
				extern bool IsNetpbm(std::wstring_view const& ext);

			}

			namespace stbi
			{
				extern bool CanReadWrite(std::string_view const& ext);
				extern bool CanReadWrite(std::wstring_view const& ext);
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

			extern std::string ConvertWString(std::wstring_view const& wstr);

			extern std::path_string_view ExtractExtensionSV(const std::filesystem::path* ext_path);
		}
}

}