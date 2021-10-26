#pragma once

#include "ImageIO.hpp"
#include <string_view>
#include <string>
#include <stb/stb_image_write.h>
#include <filesystem>

namespace img
{
	namespace io
	{


		struct WriteInfo
		{
			bool is_default=true;

			int magic_number=0;
		};

		bool writeFile(byte* data, size_t size, const wchar_t* path);

		namespace netpbm
		{

			__forceinline size_t headerSize(int magic_number, std::string const& width, std::string const& height, const std::string& max_value = "")
			{
				return
					3 + // magic number
					width.size() + 1 + 
					height.size() + 1 + 
					(((magic_number == 1) | (magic_number == 4)) ? 0 : (max_value.size() + 1));
			}

			__forceinline size_t contentSize(int magic_number, size_t number_of_pixels)
			{
				size_t size_per_pixel;
				if (magic_number == 6)
				{
					// 1 byte pdf channel, 3 channel per pixels
					size_per_pixel = 3;
				}
				else
				{
					size_per_pixel = 100;
				}
				return size_per_pixel * number_of_pixels;
			}

			__forceinline void writeHeader(byte*& ptr, int magic_number, std::string const& width, std::string const& height, const std::string & max_value="")
			{
				ptr[0] = 'P';
				ptr[1] = '0' + (char)magic_number;
				ptr[2] = '\n';
				ptr += 3;
				const auto write = [](std::string const& str, byte*& ptr)
				{
					for (int i = 0; i < str.size(); ++i)
					{
						*ptr = str[i];
						++ptr;
					}
					*ptr = ' ';
					++ptr;
				};

				write(width, ptr);
				
				write(height, ptr);

				if ((magic_number != 1) & (magic_number != 4))
				{
					write(max_value, ptr);
				}

				*(ptr - 1) = '\n';
			}

			template <class T, bool RM=IMAGE_ROW_MAJOR>
			bool write(Image<T, RM> const& img, const wchar_t* path, WriteInfo info)
			{
				std::string width = std::to_string(img.width());
				std::string height = std::to_string(img.height());
				std::string max_value = "255";

				int magic_number = [&info]
				{
					if (info.is_default)
					{
						if constexpr (math::Is_Vector<T>::value)
						{
							return 6;
						}
						else
						{
							return 5;
						}
					}
					return info.magic_number;
				}();

				size_t header_size = headerSize(info.magic_number, width, height, max_value);
				size_t content_size = contentSize(info.magic_number, img.size());
				size_t total_size = header_size + content_size;
				std::vector<byte> file_data(total_size);
				byte* ptr = file_data.data();
				const byte * const _ptr = ptr;
				writeHeader(ptr, info.magic_number, width, height, max_value);
				assert( (ptr - _ptr) == header_size);
				if (info.magic_number == 6)
				{
					Image<RGBu, IMAGE_ROW_MAJOR> tmp = img;
					std::memcpy(ptr, tmp.begin(), content_size);
				}

				bool res;
				try
				{
					res = writeFile(file_data.data(), total_size, path);
				}
				catch (std::exception const& e)
				{
					std::wcerr << L"ImWrite PPM, could not write " << path << L" because: " << e.what() << '\n';
					return false;
				}
				return res;
			}
		}
		
		namespace stbi
		{
			struct WriteContext
			{
				const wchar_t* path;
			};

			void writeFileCallback(void * context, void * data, int len);

			template <class T, bool RM = IMAGE_ROW_MAJOR>
			bool write(Image<T, RM> const& img, std::filesystem::path const& path)
			{
				if constexpr (RM == IMAGE_COL_MAJOR)
				{
					Image<T, IMAGE_ROW_MAJOR> tmp = img;
					return write(tmp);
				}

				return false;
			}

			template <int N>
			bool write(Image<math::Vector<N, uint8_t>> const& img, std::filesystem::path const& path)
			{
				const auto ext = path.extension();
				WriteContext context{
					.path = path.c_str(),
				};
				if (ext == ".png")
				{
					size_t stride = img.width() * N * sizeof(uint8_t);
					stbi_write_png_to_func(writeFileCallback, &context, img.width(), img.height(), N, img.rawData(), stride);
					//stbi_write_png(path.c_str(), img.width(), img.height(), 3, img.data(), 3);
				}
				// else if (ext == ".jpg" || ext == ".jpeg") // TODO
				return true;
			}
		}

		template <class T, bool RM=IMAGE_ROW_MAJOR>
		bool write(Image<T, RM> const& img, std::filesystem::path const& path, WriteInfo info = WriteInfo{})
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
					return netpbm::write(img, path.c_str(), info);
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