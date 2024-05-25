#pragma once


#include "ImageIO.hpp"
#include <string_view>
#include <string>

#include <core/Result.hpp>

#include <filesystem>

namespace that
{
	namespace img
	{
		namespace io
		{

			struct WriteInfo
			{
				int magic_number = -1;
				int quality = -1; // -1 means no quality loss, mainly for jpg
				int compression = -1; // -1 means no compression, mainly for png
			};

			extern Result writeFile(byte* data, size_t size, std::filesystem::path const& path);

			namespace netpbm
			{
				extern Result write(FormatlessImage const& img, FormatInfo const& format, bool row_major, std::filesystem::path const& path, WriteInfo const& info);

				extern Result write(FormatedImage const& img, std::filesystem::path const& path, WriteInfo const& info);
			
				template <class T, bool RM=IMAGE_ROW_MAJOR>
				inline Result write(Image<T, RM> const& img, std::filesystem::path const& path, WriteInfo const& info)
				{
					FormatInfo format = FormatInfo::Deduce<T>();
					return write(img, format, RM, path, info);
				}
			}
		
			namespace stbi
			{
				extern Result write(FormatlessImage const& img, FormatInfo const& format, bool row_major, std::filesystem::path const& path, WriteInfo const& info);

				extern Result write(FormatedImage const& img, std::filesystem::path const& path, WriteInfo const& info);
		
				template <class T, bool RM=IMAGE_ROW_MAJOR>
				inline Result write(Image<T, RM> const& img, std::filesystem::path const& path, WriteInfo const& info)
				{
					FormatInfo format = FormatInfo::Deduce<T>();
					return write(img, format, RM, path, info);
				}
			}

			extern Result write(FormatlessImage const& img, FormatInfo const& format, bool row_major, std::filesystem::path const& path, WriteInfo const& info);
			
			extern Result write(FormatlessImage && img, FormatInfo const& format, bool row_major, std::filesystem::path const& path, WriteInfo const& info);

			extern Result write(FormatedImage const& img, std::filesystem::path const& path, WriteInfo const& info = WriteInfo{});
			
			extern Result write(FormatedImage && img, std::filesystem::path const& path, WriteInfo const& info = WriteInfo{});

			template <class T, bool RM=IMAGE_ROW_MAJOR>
			inline Result write(Image<T, RM> const& img, std::filesystem::path const& path, WriteInfo const& info = WriteInfo{})
			{
				FormatInfo format = FormatInfo::Deduce<T>();
				return write(img, format, RM, path, info);
			}
		}
	}
}