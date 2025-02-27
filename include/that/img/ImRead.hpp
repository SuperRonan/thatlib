#pragma once

#include "ImageIO.hpp"
#include <stb/stb_image.h>

#include <that/IO/FileSystem.hpp>

namespace that
{
	namespace img
	{
		namespace io
		{
			
			struct ReadImageInfo
			{
				FileSystem::Hint hint = FileSystem::Hint::None;
				bool path_is_native = false;
				const FileSystem::Path* path = nullptr; // required
				FileSystem* filesystem = nullptr; // optional
				FormatedImage* target = nullptr; // required
			};
			Result ReadFormatedImage(ReadImageInfo const&);

			inline FormatedImage ReadFormatedImage(std::filesystem::path const& path)
			{
				ReadImageInfo info = {};
				FormatedImage res;
				info.path = &path;
				info.target = &res;
				Result result = ReadFormatedImage(info);
				return res;
			}
			
			namespace netpbm
			{
				struct Header
				{
					std::string magic_number = "";
					int width = 0, height = 0, max_value = 0;

					bool valid()const
					{
						return max_value > 0;
					}

					bool validMagicNumber()const
					{
						return magic_number.size() == 2 && magic_number[0] == 'P' && (magic_number[1] >= '1' && magic_number[1] <= '3');
					}
				};

				Result ReadFormatedImage(ReadImageInfo const& info);

				//template <class T, bool RM = IMAGE_ROW_MAJOR>
				//Image<T, RM> read(const wchar_t* name, T T_max)
				//{
				//	if constexpr (RM == IMAGE_COL_MAJOR)
				//	{
				//		return read<T, !RM>(name, T_max);
				//	}
				//	using RGBu = RGB<unsigned char>;
				//	using RGBd = RGB<unsigned char>;
				//	Image<T, RM> res;
				//	std::vector<byte> file;
				//	std::string error;
				//	try
				//	{
				//		file = load_file(name);
				//	}
				//	catch (std::exception const& e)
				//	{
				//		error = e.what();
				//	}

				//	if (error.empty())
				//	{
				//		Header header;
				//		pbyte ptr = file.data();
				//		const pbyte end = ptr + file.size();

				//		try
				//		{
				//			eat_comment(ptr, end);
				//			eat_white(ptr, end);
				//			int mode = 0;
				//			if (ptr + 2 < end && ptr[0] == 'P')
				//			{
				//				mode = ptr[1] - '0';
				//				ptr += 2;
				//			}

				//			// get w
				//			eat_comment(ptr, end);
				//			header.width = eat_int(ptr, end);

				//			// get h
				//			eat_comment(ptr, end);
				//			header.height = eat_int(ptr, end);

				//			double max_value;
				//			if ((mode == 1 || mode == 4))
				//			{
				//				max_value = 1;
				//			}
				//			else
				//			{
				//				eat_comment(ptr, end);
				//				header.max_value = eat_int(ptr, end);
				//				max_value = header.max_value;
				//			}

				//			res = Image<T, RM>(header.width, header.height);
				//			eat_comment(ptr, end);
				//			if (mode == 6)
				//			{
				//				Image<RGBu, IMAGE_ROW_MAJOR> tmp(res.width(), res.height());
				//				assert((end - ptr) >= tmp.byteSize());
				//				std::memcpy(tmp.begin(), ptr, tmp.byteSize());
				//				if constexpr (std::is_convertible<RGBu, T>::value)
				//				{
				//					res = tmp;
				//				}
				//				else
				//				{
				//					std::cout << "Cannot convert image format" << std::endl;
				//				}
				//			}
				//			else if (mode == 3)
				//			{
				//				for (int i = 0; i < res.size(); i++)
				//				{
				//					RGBd ppm_pixel;
				//					ppm_pixel[0] = eat_int(ptr, end);
				//					ppm_pixel[1] = eat_int(ptr, end);
				//					ppm_pixel[2] = eat_int(ptr, end);
				//					eat_comment(ptr, end);
				//					T t;
				//					if constexpr (math::Is_Vector<T>::value)
				//						t = (ppm_pixel / max_value) * T_max;
				//					else
				//						t = (ppm_pixel[0] / max_value) * T_max;
				//					res[i] = t;
				//				}
				//			}
				//			else if (mode == 2 || mode == 1)
				//			{
				//				for (int i = 0; i < res.size(); i++)
				//				{
				//					double ppm_pixel;
				//					ppm_pixel = eat_int(ptr, end);
				//					eat_comment(ptr, end);
				//					T t = (ppm_pixel / max_value) * T_max;
				//					res[i] = t;
				//				}
				//			}
				//			else if (mode == 5)
				//			{
				//				Image<byte, true> tmp(res.width(), res.height());
				//				std::copy(ptr, end, tmp.begin());
				//				res = std::move(tmp);
				//			}
				//			else
				//			{
				//				throw std::runtime_error("Unsupported magic number");
				//			}
				//		}
				//		catch (std::exception const& e)
				//		{
				//			error = e.what();
				//		}
				//	}

				//	if (!error.empty())
				//	{
				//		std::cerr << "Read PPM of " << (const char*)name << ": " << error << std::endl;
				//	}
				//	return res;
				//}

			
			}

			namespace stbi
			{
				Result ReadFormatedImage(ReadImageInfo const& info);

				//template <class T, bool RM = IMAGE_ROW_MAJOR>
				//Image<T, RM> read(const wchar_t* path)
				//{
				//	if constexpr (RM == IMAGE_COL_MAJOR)
				//	{
				//		Image<T, IMAGE_ROW_MAJOR> tmp = read<T, IMAGE_ROW_MAJOR>(path);
				//		return tmp;
				//	}
				//	int width, height, number_of_channels;
				//	byte* data;
				//	std::string spath = ConvertWString(path);
				//	try
				//	{
				//		data = (byte*)stbi_load(spath.c_str(), &width, &height, &number_of_channels, 0);
				//	}
				//	catch(std::exception const& e)
				//	{
				//		std::cerr << "STB could not open " << spath << " : " << e.what() << std::endl;
				//		if (data)
				//			stbi_image_free(data);
				//		return Image<T, RM>();
				//	}
				//	if (width <= 0 || height <= 0 || number_of_channels <= 0)
				//	{
				//		std::cerr<<"STB could not open the image: "<< stbi_failure_reason();
				//		Image<T, RM> res;
				//		return res;
				//	}
				//	Image<T, RM> res(width, height);

				//	constexpr const bool T_is_byte = std::is_same<byte, T>::value;

				//	if (number_of_channels == 1)
				//	{
				//		Image<byte, RM> tmp(width, height);
				//		std::memcpy(tmp.rawData(), data, width * height);
				//		res = tmp;
				//	}
				//	else if (number_of_channels == 3)
				//	{
				//		if constexpr (T_is_byte)
				//		{
				//			std::cerr << "Cannot convert image format" << std::endl;
				//		}
				//		else
				//		{
				//			Image<RGBu, RM> tmp(width, height);
				//			tmp.setData(data);
				//			res = tmp;
				//		}
				//	}
				//	else if (number_of_channels == 4)
				//	{
				//		if constexpr (T_is_byte)
				//		{
				//			std::cerr << "Cannot convert image format" << std::endl;
				//		}
				//		else
				//		{
				//			Image<RGBAu, RM> tmp(width, height);
				//			tmp.setData(data);
				//			res = tmp;
				//		}
				//	}
				//	if(data)
				//	{
				//		stbi_image_free(data);
				//	}
				//
				//	return res;
				//}
			}
		
			

			//template <class T, bool RM = true>
			//Image<T, RM> read(std::filesystem::path const& path)
			//{
			//	if (path.has_extension())
			//	{
			//		std::filesystem::path ext = path.extension();
			//		if (netpbm::isNetpbm(ext))
			//		{
			//			return netpbm::read<T, RM>(path.c_str(), TypeMax<T>());
			//		}
			//		else if (stbi::canReadWrite(ext))
			//		{
			//			return stbi::read<T, RM>(path.c_str());
			//		}
			//		else
			//		{
			//			std::cerr << "ImRead: unknown extention \"" << ext << "\" of " << path << '\n';
			//		}
			//	}
			//	else
			//	{
			//		std::cerr << "ImRead: file " << path << " has no extension, could not read it!\n";
			//	}
			//	return Image<T, RM>();
			//}

		}
	}
}