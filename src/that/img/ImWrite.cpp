#include <that/img/ImWrite.hpp>

#include <fstream>

#include <stb/stb_image_write.h>
#include <that/math/Half.hpp>

#include <that/core/Strings.hpp>

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

				Result Write(FormatlessImage const& img, FormatInfo const& format, bool row_major, std::filesystem::path const& path, WriteInfo const& info)
				{
					if (!row_major)
					{
						return Result::InvalidValue;
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
						return Result::InvalidValue;
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

					Result result = Result::Success;
					try
					{
						if (info.create_folder_ifn)
						{
							result = that::io::CreateDirectoryIFN(path);
							if (result != Result::Success)
							{
								return result;
							}
						}
						result = WriteFile(file_data.data(), total_size, path);
					}
					catch (std::exception const& e)
					{
						std::wcerr << L"ImWrite PPM, could not write " << path << L" because: " << e.what() << '\n';
						result = Result::FileWriteException;
					}
					return result;
				}

				Result Write(FormatedImage const& img, std::filesystem::path const& path, WriteInfo const& info)
				{
					return ::that::img::io::stbi::Write(img, img.format(), img.rowMajor(), path, info);
				}
			}

			namespace stbi
			{
				struct WriteContext
				{
					std::ofstream & file;
					Result result;
				};
			
				void writeFileCallback(void* context, void* data, int len)
				{
					WriteContext* _context = (stbi::WriteContext*)context;
					if (_context->result == Result::Success)
					{
						_context->file.write((const char *)data, static_cast<std::streamsize>(len));
						if (!_context->file.good())
						{
							_context->result = Result::FileWriteError;
						}
					}
				}
			
				Result Write(FormatlessImage const& img, FormatInfo const& format, bool row_major, std::filesystem::path const& path, WriteInfo const& info)
				{
					Result result = Result::Success;
					assert(path.has_extension());
					const std::filesystem::path ext = path.extension();

					if (info.create_folder_ifn)
					{
						result = that::io::CreateDirectoryIFN(path);
						if (result != Result::Success)
						{
							return result;
						}
					}

					std::ofstream file(path.c_str(), std::ios::binary | std::ios::out);
					if (!file.is_open() || !file.good())
					{
						result = Result::FileWriteError;
						return result;
					}

					WriteContext context{
						.file = file,
						.result = Result::Success,
					};

					// Hope this code is not used by stbi
					const int did_not_write = 196;
					int stbi_res = did_not_write;

					int comp = format.channels;
					if (ext == ".png")
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
					if(stbi_res == did_not_write) context.result = Result::WrongFileFormat;
					else if(stbi_res == 0)	context.result = Result::STBInteralError;
					else if (context.result == Result::Success)
					{
						file.flush();
						file.close();
					}
					result = context.result;
					return result;
				}

				Result Write(FormatedImage const& img, std::filesystem::path const& path, WriteInfo const& info)
				{
					return that::img::io::stbi::Write(img, img.format(), img.rowMajor(), path, info);
				}
			}

			enum class WriterLibrary
			{
				NETPBM,
				STBI,
				OPENEXR,
			};

			Result CheckExtension(FormatInfo const& format, bool row_major, std::filesystem::path const& path, WriteInfo const& info,
				std::filesystem::path& path_with_extension, const std::filesystem::path*& write_path, bool & need_format_conversion, FormatInfo & write_format, bool & write_major
			)
			{
				const bool can_exr = false;
				if (!path.has_extension())
				{
					path_with_extension = path;
					switch (format.type)
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
							if (format.elem_size == sizeof(float))
							{
								path_with_extension += ".hdr";
							}
							else if (format.elem_size == sizeof(double))
							{
								path_with_extension += ".hdr";
								need_format_conversion = true;
								write_format.elem_size = sizeof(float);
							}
							else if (format.elem_size == (sizeof(math::float16_t))) 
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
				return Result::Success;
			}

			Result FindFormatConversionIFP(FormatInfo const& format, bool row_major, std::filesystem::path const& extension, WriteInfo const& info,
			bool& need_format_conversion, FormatInfo& write_format, bool& write_major,
				WriterLibrary writer
			)
			{
				Result res = Result::Success;
				if (!need_format_conversion)
				{
					// it might need one
					if (writer == WriterLibrary::NETPBM)
					{
						res = Result::NotImplemented;
					}
					else if (writer == WriterLibrary::STBI)
					{
						if (row_major != IMAGE_ROW_MAJOR)
						{
							need_format_conversion = true;
							write_major = IMAGE_ROW_MAJOR;
						}
						switch (format.type)
						{
							case ElementType::UNORM:
							case ElementType::SNORM:
							case ElementType::UINT:
							case ElementType::SINT:
							case ElementType::sRGB:
							{
								if (extension == ".hdr")
								{
									need_format_conversion = true;
									write_format.elem_size = sizeof(float);
									write_format.type = ElementType::FLOAT;
								}
								else
								{
									if (format.elem_size != 1)
									{
										need_format_conversion = true;
										write_format.elem_size = 1;
									}
								}
							}
							break;
							case ElementType::FLOAT:
							{
								if (extension == ".hdr")
								{
									// stbi .hdr can only write float
									if (format.elem_size != sizeof(float))
									{
										need_format_conversion = true;
										write_format.elem_size = sizeof(float);
									}
								}
								else
								{
									write_format.elem_size = 1;
									write_format.type = ElementType::UNORM;
									need_format_conversion = true;
								}
							}
						}
					}
					else if (writer == WriterLibrary::OPENEXR)
					{
						// openEXR can write float or half
						if (format.elem_size == sizeof(double) || format.type != ElementType::FLOAT)
						{
							need_format_conversion = true;
							write_format.elem_size = sizeof(float);
							write_format.type = ElementType::FLOAT;
						}
						res = Result::NotImplemented;
					}
				}
				return res;
			}

			Result CheckWrite(FormatInfo const& format, bool row_major, std::filesystem::path const& path, WriteInfo const& info,
				std::filesystem::path& path_with_extension, const std::filesystem::path*& write_path, bool& need_format_conversion, FormatInfo& write_format, bool& write_major,
				WriterLibrary& writer)
			{
				Result res = Result::Success;
				const bool can_exr = false;
				res = CheckExtension(format, row_major, path, info, path_with_extension, write_path, need_format_conversion, write_format, write_major);
				
				if (res != Result::Success)
				{
					return res;
				}

				if (!write_path->has_extension())
				{
					return Result::InvalidValue;
				}

				const std::filesystem::path ext_path = write_path->extension();
				that::PathStringView ext = ExtractExtensionSV(&ext_path);
				if (netpbm::IsNetpbm(ext))
				{
					writer = WriterLibrary::NETPBM;
				}
				else if (stbi::CanReadWrite(ext))
				{
					writer = WriterLibrary::STBI;
				}
				else if (can_exr && false)
				{
					writer = WriterLibrary::OPENEXR;
				}

				res = FindFormatConversionIFP(format, row_major, ext_path, info, need_format_conversion, write_format, write_major, writer);

				return res;
			}

			
			Result Write(FormatlessImage const& img, FormatInfo const& format, bool row_major, std::filesystem::path const& path, WriteInfo const& info,
				std::function<Result(FormatInfo const& write_format, bool write_major, const FormatlessImage *& target)> convert_format_f
			)
			{
				Result res = Result::Success;
				if (img.empty() || path.empty())
				{
					return Result::InvalidValue;
				}
				const FormatlessImage * target = &img;

				const std::filesystem::path * write_path = &path;
				std::filesystem::path path_with_extension;

				bool need_format_conversion = false;
				FormatInfo write_format = format;
				bool write_major = row_major;
				WriterLibrary writer;

				res = CheckWrite(format, row_major, path, info, path_with_extension, write_path, need_format_conversion, write_format, write_major, writer);

				if (res != Result::Success)
				{
					return res;
				}

				FormatedImage write_img;
				if (need_format_conversion)
				{
					res = convert_format_f(write_format, write_major, target);
					if (res != Result::Success)
					{
						return res;
					}
				}

				if (writer == WriterLibrary::NETPBM)
				{
					res = netpbm::Write(*target, write_format, write_major, *write_path, info);
				}
				else if (writer == WriterLibrary::STBI)
				{
					res = stbi::Write(*target, write_format, write_major, *write_path, info);
				}
				else if (writer == WriterLibrary::OPENEXR)
				{
					res = Result::NotImplemented;
				}
				return res;
			}

			Result Write(FormatlessImage const& img, FormatInfo const& format, bool row_major, std::filesystem::path const& path, WriteInfo const& info)
			{
				FormatlessImage reformated;
				return Write(img, format, row_major, path, info, [&](FormatInfo const& write_format, bool write_major, const FormatlessImage* target) -> Result
				{
					reformated = FormatlessImage(img.width(), img.height(), write_format.pixelSize());
					target = &reformated;
					Result result = reformated.convertFormat(format, row_major, write_format, write_major);
					return result;
				});
			}

			Result Write(FormatedImage const& img, std::filesystem::path const& path, WriteInfo const& info)
			{
				return Write(img, img.format(), img.rowMajor(), path, info);
			}

			Result Write(FormatlessImage&& img, FormatInfo const& format, bool row_major, std::filesystem::path const& path, WriteInfo const& info,
				std::function<Result(FormatInfo const& write_format, bool write_major)> convert_format_f
			)
			{
				Result res = Result::Success;
				if (img.empty() || path.empty())
				{
					return Result::InvalidValue;
				}

				const std::filesystem::path* write_path = &path;
				std::filesystem::path path_with_extension;

				bool need_format_conversion = false;
				FormatInfo write_format = format;
				bool write_major = row_major;
				WriterLibrary writer;

				res = CheckWrite(format, row_major, path, info, path_with_extension, write_path, need_format_conversion, write_format, write_major, writer);

				if (res != Result::Success)
				{
					return res;
				}

				if (need_format_conversion)
				{
					res = convert_format_f(write_format, write_major);
					if (res != Result::Success)
					{
						return res;
					}
				}

				if (writer == WriterLibrary::NETPBM)
				{
					res = netpbm::Write(img, write_format, write_major, *write_path, info);
				}
				else if (writer == WriterLibrary::STBI)
				{
					res = stbi::Write(img, write_format, write_major, *write_path, info);
				}
				else if (writer == WriterLibrary::OPENEXR)
				{
					res = Result::NotImplemented;
				}
				return res;
			}

			Result Write(FormatlessImage&& img, FormatInfo const& format, bool row_major, std::filesystem::path const& path, WriteInfo const& info)
			{
				return Write(std::move(img), format, row_major, path, info, [&](FormatInfo const& write_format, bool write_major)
				{
					return img.convertFormat(format, row_major, write_format, write_major);
				});
			}

			Result Write(FormatedImage&& img, std::filesystem::path const& path, WriteInfo const& info)
			{
				return Write(std::move(img), img.format(), img.rowMajor(), path, info, [&](FormatInfo const& write_format, bool write_major)
				{
					return img.reFormat(write_format, write_major);
				});
			}

			Result WriteFile(byte* data, size_t size, std::filesystem::path const& path)
			{
				Result res = Result::Success;
				std::ofstream file(path.c_str(), std::ios::binary | std::ios::out);
				if (file.good() && file.is_open())
				{
					file.write((const char*)data, size);
					file.flush();
					file.close();
					res = Result::Success;
				}
				else
				{
					res = Result::FileWriteError;
				}
				return res;
			}

		
		}


	}
}