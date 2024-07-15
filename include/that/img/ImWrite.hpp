#pragma once


#include "ImageIO.hpp"
#include <string_view>
#include <string>

#include <that/core/Result.hpp>

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
				bool create_folder_ifn = false;
			};

			extern Result WriteFile(byte* data, size_t size, std::filesystem::path const& path);

			namespace netpbm
			{
				extern Result Write(FormatlessImage const& img, FormatInfo const& format, bool row_major, std::filesystem::path const& path, WriteInfo const& info);

				extern Result Write(FormatedImage const& img, std::filesystem::path const& path, WriteInfo const& info);
			
				template <class T, bool RM=IMAGE_ROW_MAJOR>
				inline Result Write(Image<T, RM> const& img, std::filesystem::path const& path, WriteInfo const& info)
				{
					FormatInfo format = FormatInfo::Deduce<T>();
					return Write(img, format, RM, path, info);
				}
			}
		
			namespace stbi
			{
				extern Result Write(FormatlessImage const& img, FormatInfo const& format, bool row_major, std::filesystem::path const& path, WriteInfo const& info);

				extern Result Write(FormatedImage const& img, std::filesystem::path const& path, WriteInfo const& info);
		
				template <class T, bool RM=IMAGE_ROW_MAJOR>
				inline Result Write(Image<T, RM> const& img, std::filesystem::path const& path, WriteInfo const& info)
				{
					FormatInfo format = FormatInfo::Deduce<T>();
					return Write(img, format, RM, path, info);
				}
			}

			extern Result Write(FormatlessImage const& img, FormatInfo const& format, bool row_major, std::filesystem::path const& path, WriteInfo const& info);
			
			extern Result Write(FormatlessImage && img, FormatInfo const& format, bool row_major, std::filesystem::path const& path, WriteInfo const& info);

			extern Result Write(FormatedImage const& img, std::filesystem::path const& path, WriteInfo const& info = WriteInfo{});
			
			extern Result Write(FormatedImage && img, std::filesystem::path const& path, WriteInfo const& info = WriteInfo{});

			template <class T, bool RM=IMAGE_ROW_MAJOR>
			inline Result Write(Image<T, RM> const& img, std::filesystem::path const& path, WriteInfo const& info = WriteInfo{})
			{
				FormatInfo format = FormatInfo::Deduce<T>();
				return Write(img, format, RM, path, info);
			}
		}
	}
}