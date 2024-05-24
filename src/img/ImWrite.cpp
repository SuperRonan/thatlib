#include "ImWrite.hpp"

#include <fstream>

#include <stb/stb_image_write.h>
#include <math/Half.hpp>

namespace that
{


	namespace img
	{
		namespace io
		{
			namespace netpbm
			{
				inline size_t headerSize(int magic_number, std::string const& width, std::string const& height, const std::string& max_value = "")
				{
					return
						3 + // magic number
						width.size() + 1 +
						height.size() + 1 +
						(((magic_number == 1) || (magic_number == 4)) ? 0 : (max_value.size() + 1));
				}

				inline size_t contentSize(int magic_number, size_t number_of_pixels)
				{
					size_t size_per_pixel;
					if (magic_number == 5)
					{
						size_per_pixel = 1;
					}
					else if (magic_number == 6)
					{
						// 1 byte per channel, 3 channel per pixels
						size_per_pixel = 3;
					}
					else
					{
						size_per_pixel = 0;
					}
					return size_per_pixel * number_of_pixels;
				}

				// TODO write in a stream, the stream maybe a pointer to a buffer, or a file

				inline void writeHeader(byte*& ptr, int magic_number, std::string const& width, std::string const& height, const std::string& max_value = "")
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

					if ((magic_number != 1) && (magic_number != 4))
					{
						write(max_value, ptr);
					}

					*(ptr - 1) = '\n';
				}

				bool write(FormatlessImage const& img, FormatInfo const& format, bool row_major, std::filesystem::path const& path, WriteInfo const& info)
				{
					if (!row_major)
					{
						return false;
						// TODO
						//FormatedImage tmp(img.width(), img.height(), format, true);
					}
					std::string width = std::to_string(img.width());
					std::string height = std::to_string(img.height());
					std::string max_value = "255";

					int magic_number = [&]
					{
						if (info.magic_number < 0)
						{
							const uint32_t c = format.channels;
							if (c == 1)
							{
								return 5;
							}
							else if(c == 3)
							{
								return 5;
							}
							else
							{
								return 7;
							}
						}
						return info.magic_number;
					}();

					if (info.magic_number < 0)
					{
						return false;
					}

					size_t header_size = headerSize(info.magic_number, width, height, max_value);
					size_t content_size = contentSize(info.magic_number, img.size());
					size_t total_size = header_size + content_size;

					std::vector<byte> file_data(total_size);
					byte* ptr = file_data.data();
					const byte* const _ptr = ptr;
					writeHeader(ptr, info.magic_number, width, height, max_value);
					assert((ptr - _ptr) == header_size);
				
					std::memcpy(ptr, img.rawData(), content_size);

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

				bool write(FormatedImage const& img, std::filesystem::path const& path, WriteInfo const& info)
				{
					return write(img, img.format(), img.rowMajor(), path, info);
				}
			}

			namespace stbi
			{
				struct WriteContext
				{
					std::ofstream & file;
					bool result;
				};
			
				void writeFileCallback(void* context, void* data, int len)
				{
					WriteContext* _context = (stbi::WriteContext*)context;
					if (_context->result)
					{
						_context->file.write((const char *)data, static_cast<std::streamsize>(len));
						_context->result &= _context->file.good();
					}
				}
			
				bool write(FormatedImage const& img, std::filesystem::path const& path, WriteInfo const& info)
				{
					return write(img, img.format(), img.rowMajor(), path, info);
				}
			
				bool write(FormatlessImage const& img, FormatInfo const& format, bool row_major, std::filesystem::path const& path, WriteInfo const& info)
				{
					assert(path.has_extension());
					const std::filesystem::path ext = path.extension();

					int comp = format.channels;
				
					std::ofstream file(path.c_str(), std::ios::binary | std::ios::out);
					if (!file.is_open() || !file.good())
					{
						return false;
					}

					WriteContext context{
						.file = file,
						.result = true,
					};

					int stbi_res = -1;

					if (ext == ".pnr")
					{
						if (format.elem_size == 1)
						{
							stbi_res = stbi_write_png_to_func(writeFileCallback, &context, img.width(), img.height(), comp, img.rawData(), 0);
						}
					}
					else if (ext == ".bmp")
					{
						if (format.elem_size == 1)
						{
							stbi_res = stbi_write_bmp_to_func(writeFileCallback, &context, img.width(), img.height(), comp, img.rawData());
						}
					}
					else if (ext == ".tga")
					{
						if (format.elem_size == 1)
						{
							stbi_res = stbi_write_tga_to_func(writeFileCallback, &context, img.width(), img.height(), comp, img.rawData());
						}
					}
					else if (ext == ".jpg")
					{
						if (format.elem_size == 1)
						{
							int quality = info.quality;
							if(quality == -1) quality = 100;
							stbi_res = stbi_write_jpg_to_func(writeFileCallback, &context, img.width(), img.height(), comp, img.rawData(), quality);
						}
					}
					else if (ext == ".hdr")
					{
						if ((format.elem_size == sizeof(float)) && format.type == ElementType::FLOAT)
						{
							stbi_res = stbi_write_hdr_to_func(writeFileCallback, &context, img.width(), img.height(), comp, reinterpret_cast<const float*>(img.rawData()));
						}
					}
					// stbi write returns 0 on failure, not 0 on success (stupid, int res should be an error code)
					if(stbi_res == 0)	context.result = false;
					if (context.result)
					{
						file.flush();
						file.close();
					}
					return context.result;
				}
			}

			enum class WriterLibrary
			{
				NETPBM,
				STBI,
				OPENEXR,
			};

			bool CheckExtension(FormatedImage const& img, std::filesystem::path const& path, WriteInfo const& info,
				std::filesystem::path& path_with_extension, const std::filesystem::path*& write_path, bool & need_format_conversion, FormatInfo & write_format
			)
			{
				const bool can_exr = false;
				if (!path.has_extension())
				{
					path_with_extension = path;
					switch (img.format().type)
					{
						case ElementType::UNORM:
						case ElementType::SNORM:
						case ElementType::UINT:
						case ElementType::SINT:
						case ElementType::sRGB:
						{
							path_with_extension += ".png";
						}
						break;
						case ElementType::FLOAT:
						{
							if (img.format().elem_size == sizeof(float))
							{
								path_with_extension += ".hdr";
							}
							else if (img.format().elem_size == sizeof(double))
							{
								path_with_extension += ".hdr";
								need_format_conversion = true;
								write_format.elem_size = sizeof(float);
							}
							else if (img.format().elem_size == (sizeof(float) / 2)) // float16_t in C++23 or use an external Half type
							{
								if (can_exr)
								{
									path_with_extension += ".exr";
								}
								else
								{
									path_with_extension += ".hdr";
									need_format_conversion = true;
									write_format.elem_size = sizeof(float);
								}
							}
						}
						break;
					}
					write_path = &path_with_extension;
				}
				return true;
			}

			bool FindFormatConversionIFP(FormatedImage const& img, std::filesystem::path const& path, WriteInfo const& info,
				std::filesystem::path& path_with_extension, const std::filesystem::path*& write_path, bool& need_format_conversion, FormatInfo& write_format,
				WriterLibrary writer
			)
			{
				bool res = true;
				if (!need_format_conversion)
				{
					// it might need one
					if (writer == WriterLibrary::NETPBM)
					{
						// TODO
					}
					else if (writer == WriterLibrary::STBI)
					{
						if (img.rowMajor() == false)
						{
							need_format_conversion = true;
						}
						switch (img.format().type)
						{
						case ElementType::UNORM:
						case ElementType::SNORM:
						case ElementType::UINT:
						case ElementType::SINT:
						case ElementType::sRGB:
						{
							if (img.format().elem_size != 1)
							{
								need_format_conversion = true;
								write_format.elem_size = 1;
							}
						}
						break;
						case ElementType::FLOAT:
						{
							// stbi .hdr can only write float
							if (img.format().elem_size != sizeof(float))
							{
								need_format_conversion = true;
								write_format.elem_size = sizeof(float);
							}
						}
						}
					}
					else if (writer == WriterLibrary::OPENEXR)
					{
						// openEXR can write float or half
						if (img.format().elem_size == sizeof(double))
						{
							need_format_conversion = true;
							write_format.elem_size = sizeof(float);
						}
					}
				}
				return res;
			}

			bool CheckWrite(FormatedImage const& img, std::filesystem::path const& path, WriteInfo const& info,
				std::filesystem::path& path_with_extension, const std::filesystem::path*& write_path, bool& need_format_conversion, FormatInfo& write_format,
				WriterLibrary& writer)
			{
				bool res = false;
				const bool can_exr = false;
				res = CheckExtension(img, path, info, path_with_extension, write_path, need_format_conversion, write_format);
				
				if (!res)
				{
					return res;
				}

				if (!write_path->has_extension())
				{
					return false;
				}

				const std::filesystem::path ext = write_path->extension();
				if (netpbm::isNetpbm(ext))
				{
					writer = WriterLibrary::NETPBM;
				}
				else if (stbi::canReadWrite(ext))
				{
					writer = WriterLibrary::STBI;
				}
				else if (can_exr && false)
				{
					writer = WriterLibrary::OPENEXR;
				}

				res = FindFormatConversionIFP(img, path, info, path_with_extension, write_path, need_format_conversion, write_format, writer);

				return res;
			}

			bool write(FormatedImage const& img, std::filesystem::path const& path, WriteInfo const& info)
			{
				bool res = false;
				if (img.empty() || path.empty())
				{
					return false;
				}
				const FormatedImage * target = &img;

				const std::filesystem::path * write_path = &path;
				std::filesystem::path path_with_extension;

				bool need_format_conversion = false;
				FormatInfo write_format = img.format();
				WriterLibrary writer;

				res = CheckWrite(img, path, info, path_with_extension, write_path, need_format_conversion, write_format, writer);

				if (!res)
				{
					return res;
				}

				FormatedImage write_img;
				if (need_format_conversion)
				{
					write_img = FormatedImage(img.width(), img.height(), write_format);
					write_img.copyReformat(img);
					target = &write_img;
				}

				if (writer == WriterLibrary::NETPBM)
				{
					res = netpbm::write(*target, *write_path, info);
				}
				else if (writer == WriterLibrary::STBI)
				{
					res = stbi::write(*target, *write_path, info);
				}
				else if (writer == WriterLibrary::OPENEXR)
				{
					// TODO
				}
				return res;
			}

			bool write(FormatedImage&& img, std::filesystem::path const& path, WriteInfo const& info)
			{
				bool res = false;
				if (img.empty() || path.empty())
				{
					return false;
				}

				const std::filesystem::path* write_path = &path;
				std::filesystem::path path_with_extension;

				bool need_format_conversion = false;
				FormatInfo write_format = img.format();
				WriterLibrary writer;

				res = CheckWrite(img, path, info, path_with_extension, write_path, need_format_conversion, write_format, writer);

				if (!res)
				{
					return res;
				}

				FormatedImage write_img;
				if (need_format_conversion)
				{
					img.reFormat(write_format);
				}

				if (writer == WriterLibrary::NETPBM)
				{
					res = netpbm::write(img, *write_path, info);
				}
				else if (writer == WriterLibrary::STBI)
				{
					res = stbi::write(img, *write_path, info);
				}
				else if (writer == WriterLibrary::OPENEXR)
				{
					// TODO
				}
				return res;
			}

			bool writeFile(byte* data, size_t size, std::filesystem::path const& path)
			{
				bool res = false;
				std::ofstream file(path.c_str(), std::ios::binary | std::ios::out);
				if (file.good() && file.is_open())
				{
					file.write((const char*)data, size);
					file.flush();
					file.close();
					res = true;
				}
				return res;
			}

		
		}


	}
}