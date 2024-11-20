#pragma once


#include "ImageIO.hpp"
#include <string_view>
#include <string>

#include <that/core/Result.hpp>
#include <that/IO/FileSystem.hpp>
namespace that
{
	namespace img
	{
		namespace io
		{

			struct WriteInfo
			{
				FileSystem::Hint hint = FileSystem::Hint::None;
				int magic_number = -1;
				int quality = -1; // -1 means no quality loss, mainly for jpg
				int compression = -1; // -1 means no compression, mainly for png
				bool path_is_native = false;
				bool row_major = false;
				bool can_modify_image = false;
				FormatInfo format = {};
				union
				{
					const FormatlessImage * const_image = nullptr; // required
					FormatlessImage * image;
				};
				const FileSystem::Path * path = nullptr; // required
				FileSystem * filesystem = nullptr; // optional
			};

			namespace netpbm
			{
				extern Result Write(WriteInfo const& info);
			
				//template <class T, bool RM=IMAGE_ROW_MAJOR>
				//inline Result Write(Image<T, RM> const& img, std::filesystem::path const& path, WriteInfo const& info)
				//{
				//	FormatInfo format = FormatInfo::Deduce<T>();
				//	return Write(img, format, RM, path, info);
				//}
			}
		
			namespace stbi
			{
				extern Result Write(WriteInfo const& info);
		
				//template <class T, bool RM=IMAGE_ROW_MAJOR>
				//inline Result Write(Image<T, RM> const& img, std::filesystem::path const& path, WriteInfo const& info)
				//{
				//	FormatInfo format = FormatInfo::Deduce<T>();
				//	return Write(img, format, RM, path, info);
				//}
			}

			extern Result Write(WriteInfo const& info);

			//template <class T, bool RM=IMAGE_ROW_MAJOR>
			//inline Result Write(Image<T, RM> const& img, std::filesystem::path const& path, WriteInfo const& info = WriteInfo{})
			//{
			//	FormatInfo format = FormatInfo::Deduce<T>();
			//	return Write(img, format, RM, path, info);
			//}
		}
	}
}