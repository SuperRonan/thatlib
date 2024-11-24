#include <that/img/ImRead.hpp>

#include <that/IO/File.hpp>

#include <fstream>
#include <cassert>

namespace that
{
	namespace img
	{
		namespace io
		{
			namespace netpbm
			{
				__forceinline void eat_white(pbyte& ptr, const pbyte end)
				{
					for (; ptr != end; ++ptr)
					{
						if (*ptr != '\n' && *ptr != '\r' && *ptr != '\t' && *ptr != ' ')
							break;
					}
				}

				__forceinline void eat_line(pbyte& ptr, const pbyte end)
				{
					for (; ptr != end; ++ptr)
					{
						if (*ptr == '\n')
							break;
					}
					ptr++;
				}

				__forceinline void eat_token(pbyte& ptr, const pbyte end)
				{
					for (; ptr != end; ++ptr)
					{
						if (*ptr == '\n' || *ptr == '\r' || *ptr == '\t' || *ptr == ' ')
							break;
					}
				}

				__forceinline int eat_int(pbyte& ptr, const pbyte end)
				{
					eat_white(ptr, end);
					int v = atoi((char*)ptr);
					eat_token(ptr, end);
					return v;
				}

				__forceinline void eat_comment(pbyte& ptr, const pbyte end)
				{
					while (ptr != end)
					{
						eat_white(ptr, end);
						if (*ptr != '#')
							break;
						eat_line(ptr, end);
					}
				}

				Result ReadFormatedImage(ReadImageInfo const& info)
				{
					Result result = Result::Success;
					std::vector<byte> file;

					if (!info.target)
					{
						result = Result::InvalidValue;
					}

					{
						FileSystem::ReadFileInfo fs_info{
							.hint = info.hint,
							.path = info.path,
							.result_vector = &file,
						};
						result = FileSystem::ReadFile(fs_info, info.filesystem);
					}

					if (result == Result::Success)
					{
						Header header;
						bool row_major = true;
						FormatInfo format;
						pbyte ptr = file.data();
						const pbyte end = ptr + file.size();

						try
						{
							eat_comment(ptr, end);
							eat_white(ptr, end);
							int mode = 0;
							if (ptr + 2 < end && ptr[0] == 'P')
							{
								mode = ptr[1] - '0';
								ptr += 2;
							}

							eat_comment(ptr, end);
							header.width = eat_int(ptr, end);

							eat_comment(ptr, end);
							header.height = eat_int(ptr, end);

							double max_value;
							if (mode == 1 || mode == 4)
							{
								max_value = 1;
								header.max_value = 1;
							}
							else
							{
								eat_comment(ptr, end);
								header.max_value = eat_int(ptr, end);
								max_value = header.max_value;
							}

							format.type = ElementType::sRGB;
							format.elem_size = 1;
							if (mode == 1 || mode == 2 || mode == 4 || mode == 5)
							{
								format.channels = 1;
							}
							else
							{
								format.channels = 3;
							}

							assert(header.max_value <= 255);

							eat_comment(ptr, end);

							*info.target = FormatedImage(header.width, header.height, format, row_major);

							if (mode <= 3) // ASCII
							{
								throw std::runtime_error("NetPBM ASCII not implemented yet");
							}
							else // Binary
							{
								std::memcpy(info.target->rawData(), ptr, info.target->byteSize());
							}
						}
						catch (std::exception const& e)
						{
							result = Result::Exception;
						}
					}

					return result;
				}
			}

			namespace stbi
			{
				Result ReadFormatedImage(ReadImageInfo const& info)
				{
					Result result = Result::Success;
					bool row_major = true;
					FormatInfo format;
					int width, height, channels;

					if (!info.target)
					{
						result = Result::InvalidParameter;
						return result;
					}

					std::vector<uint8_t> file;
					{
						FileSystem::ReadFileInfo fs_info{
							.hint = info.hint,
							.path = info.path,
							.result_vector = &file,
						};
						result = FileSystem::ReadFile(fs_info, info.filesystem);
					}

					uint8_t * data = nullptr;
					if (result == Result::Success)
					{
						try
						{
							data = stbi_load_from_memory(file.data(), file.size(), &width, &height, &channels, 0);
						}
						catch (std::exception const& e)
						{
							result = Result::STBInteralError;
							if (data)
							{
								stbi_image_free(data);
							}
						}
					}

					if (result == Result::Success)
					{
						if (width <= 0 || height <= 0 || channels <= 0)
						{
							result = Result::InvalidValue;
						}
						else
						{
							format.type = ElementType::UNORM;
							format.elem_size = 1;
							format.channels = channels;

							*info.target = FormatedImage(width, height, format, row_major);

							std::memcpy(info.target->rawData(), data, info.target->byteSize());
						}
					}

					if (data)
					{
						stbi_image_free(data);
					}

					return result;
				}
			}

			Result ReadFormatedImage(ReadImageInfo const& info)
			{
				Result result = Result::Success;
				if (!info.path)
				{
					result = Result::InvalidParameter;
					return result;
				}
				if (info.path->has_extension())
				{
					const std::filesystem::path ext_path = info.path->extension();
					that::PathStringView ext = ExtractExtensionSV(&ext_path);
					if (netpbm::IsNetpbm(ext))
					{
						result = netpbm::ReadFormatedImage(info);
						result = netpbm::ReadFormatedImage(info);
					}
					else if (stbi::CanReadWrite(ext))
					{
						result = stbi::ReadFormatedImage(info);
					}
					else
					{
						result = Result::UnknownFileExtension;
					}
				}
				else
				{
					result = Result::InvalidParameter;
				}
				return result;
			}
		}
	}

}