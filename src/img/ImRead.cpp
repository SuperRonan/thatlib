#include "ImRead.hpp"

#include <cassert>

namespace that
{


	namespace img
	{
		namespace io
		{
			std::vector<byte> load_file(const wchar_t* name)
			{
				std::vector<byte> buf;
				// open
				FILE* f;
				_wfopen_s(&f, name, L"rb");
				if (!f)
				{
					throw std::runtime_error(std::string("Could not open file: ") + (const char*)name);
				}

				// get size
				fseek(f, 0, SEEK_END);
				int s = ftell(f);
				fseek(f, 0, SEEK_SET);

				// read (put space at end for atoi)
				buf.resize(s + 1);
				fread((char*)&buf[0], 1, s, f);
				buf[s] = ' ';

				// close
				fclose(f);
				return buf;
			}

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

				FormatedImage readFormatedImage(const std::filesystem::path& path)
				{
					std::vector<byte> file;
					std::string error;
					try
					{
						file = load_file(path.c_str());
					}
					catch (std::exception const& e)
					{
						error = e.what();
					}

					if (error.empty())
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

							FormatedImage res(header.width, header.height, format, row_major);

							if (mode <= 3) // ASCII
							{
								throw std::runtime_error("NetPBM ASCII not implemented yet");
							}
							else // Binary
							{
								std::memcpy(res.rawData(), ptr, res.byteSize());
							}

							return res;
						}
						catch (std::exception const& e)
						{
							error = e.what();
						}
					}

					return FormatedImage();
				}
			}

			namespace stbi
			{
				FormatedImage readFormatedImage(std::filesystem::path const& path)
				{
					bool row_major = true;
					FormatInfo format;
					int width, height, channels;

					byte * data = nullptr;
					try
					{
						data = (byte*) stbi_load(path.string().c_str(), &width, &height, &channels, 0);
					}
					catch (std::exception const& e)
					{
						std::cerr << "STB could not open " << path << " : " << e.what() << std::endl;
						if (data)
							stbi_image_free(data);
						return FormatedImage();
					}

					if (width <= 0 || height <= 0 || channels <= 0)
					{
						std::cerr << "STB could not open the image: " << path << ", " << stbi_failure_reason() << std::endl;
						return FormatedImage();
					}
					format.type = ElementType::UNORM;
					format.elem_size = 1;
					format.channels = channels;

					FormatedImage res(width, height, format, row_major);

					std::memcpy(res.rawData(), data, res.byteSize());

					if (data)
					{
						stbi_image_free(data);
					}

					return res;
				}
			}

			FormatedImage readFormatedImage(std::filesystem::path const& path)
			{
				if (path.has_extension())
				{
					const std::filesystem::path ext_path = path.extension();
					std::path_string_view ext = extractExtensionSV(&ext_path);
					if (netpbm::isNetpbm(ext))
					{
						return netpbm::readFormatedImage(path);
					}
					else if (stbi::canReadWrite(ext))
					{
						return stbi::readFormatedImage(path);
					}
					else
					{
						std::cerr << "ImRead: unknown extention \"" << ext_path << "\" of " << path << '\n';
					}
				}
				else
				{
					std::cerr << "ImRead: file " << path << " has no extension, could not read it!\n";
				}
				return FormatedImage();
			}
		}
	}

}