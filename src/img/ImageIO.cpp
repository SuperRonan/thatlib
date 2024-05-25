#include <img/ImageIO.hpp>

namespace that
{
	namespace img
	{
		namespace io
		{
		
			namespace netpbm
			{
				bool isNetpbm(std::string_view const& ext)
				{
					return ext == "ppm" || ext == "pgm" || ext == "pbm";
				}

				bool isNetpbm(std::wstring_view const& ext)
				{
					return ext == L"ppm" || ext == L"pgm" || ext == L"pbm";
				}
			}

			namespace stbi
			{
				bool canReadWrite(std::string_view const& ext)
				{
					return ext == "png" || ext == "jpg" || ext == "jpeg" || ext == "tga" || ext == "hdr";
				}

				bool canReadWrite(std::wstring_view const& ext)
				{
					return ext == L"png" || ext == L"jpg" || ext == L"jpeg" || ext == L"tga" || ext == L"hdr";
				}
			}

			std::string convertWString(std::wstring_view const& wstr)
			{
				std::string res;
				res.resize(wstr.size());
				std::transform(wstr.begin(), wstr.end(), res.begin(), [](wchar_t wc) {return static_cast<char>(wc); });
				return res;
				//return std::string(wstr.begin(), wstr.end());
			}

			// ext_path = some_path.extension();
			// The pointer guarantees ext_path is not an l-value
			// std::filesystem::path::extension() should return a string_view on the ext
			std::path_string_view extractExtensionSV(const std::filesystem::path * ext_path)
			{
				std::path_string const& s = ext_path->native();
				assert(s.front() == '.');
				std::path_string_view ext = std::path_string_view(s.data() + 1, s.size() - 1);
				return ext;
			}
		}
}

}