#pragma once

#include "ImageIO.hpp"
#include <string_view>
#include <string>

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

			bool writeFile(byte* data, size_t size, std::filesystem::path const& path);

			namespace netpbm
			{
				bool write(FormatlessImage const& img, FormatInfo const& format, bool row_major, std::filesystem::path const& path, WriteInfo const& info);

				bool write(FormatedImage const& img, std::filesystem::path const& path, WriteInfo const& info);
			
				template <class T, bool RM=IMAGE_ROW_MAJOR>
				bool write(Image<T, RM> const& img, std::filesystem::path const& path, WriteInfo const& info)
				{
					FormatInfo format = FormatInfo::Deduce<T>();
					return write(img, format, RM, path, info);
				}
			}
		
			namespace stbi
			{
				bool write(FormatlessImage const& img, FormatInfo const& format, bool row_major, std::filesystem::path const& path, WriteInfo const& info);

				bool write(FormatedImage const& img, std::filesystem::path const& path, WriteInfo const& info);
		
				template <class T, bool RM=IMAGE_ROW_MAJOR>
				bool write(Image<T, RM> const& img, std::filesystem::path const& path, WriteInfo const& info)
				{
					FormatInfo format = FormatInfo::Deduce<T>();
					return write(img, format, RM, path, info);
				}
			}

			bool write(FormatedImage const& img, std::filesystem::path const& path, WriteInfo const& info = WriteInfo{});
			
			bool write(FormatedImage && img, std::filesystem::path const& path, WriteInfo const& info = WriteInfo{});

			template <class T, bool RM=IMAGE_ROW_MAJOR>
			bool write(Image<T, RM> const& img, std::filesystem::path const& path, WriteInfo const& info = WriteInfo{})
			{
				if (img.empty())
				{
					std::cerr << "ImWrite: Cannot write a empty image to " << path << '\n';
					return false;
				}
				if (path.has_extension())
				{
					std::filesystem::path ext = path.extension();
					if (netpbm::isNetpbm(ext))
					{
						return netpbm::write(img, path, info);
					}
					else if (stbi::canReadWrite(ext))
					{
						return stbi::write(img, path);
					}
					else
					{
						std::cerr << "ImWrite: unknown extention \"" << ext << "\" of " << path << '\n';
						return false;
					}
				}
				else
				{
					std::cerr<<"ImWrite: file " << path << " has no extension, could not read it!\n";
					return false;
				}
			}
		}
	}
}