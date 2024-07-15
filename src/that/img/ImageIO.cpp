#include <that/img/ImageIO.hpp>

namespace that
{
	namespace io
	{
		
		Result CreateFolderIFNRec(std::filesystem::path const& path)
		{
			Result result = Result::Success;
			if (std::filesystem::exists(path))
			{
				result = Result::Success;
			}
			else if (path.root_path() == path)
			{
				// Cannot create root
				result = Result::IOError;
			}
			else
			{
				if (path.has_parent_path())
				{
					std::filesystem::path p = path.parent_path();
					result = CreateFolderIFNRec(p);
					if (result == Result::Success)
					{
						bool b = std::filesystem::create_directory(path);
						if (b || std::filesystem::is_directory(path))
						{
							result = Result::Success;
						}
						else
						{
							result = Result::IOError;
						}
					}
				}
				else
				{
					result = Result::InvalidValue;
				}
			}
			return result;
		}

		Result CreateDirectoryCannonicalIFN(std::filesystem::path const& cp)
		{
			std::filesystem::path p = cp;
			if (p.has_filename())
			{
				p = p.remove_filename();
			}
			return CreateFolderIFNRec(p);
		}

		Result CreateDirectoryIFN(std::filesystem::path const& path)
		{
			std::filesystem::path cp = std::filesystem::weakly_canonical(path);
			return CreateDirectoryCannonicalIFN(cp);	
		}
	}

	namespace img
	{
		namespace io
		{
		
			namespace netpbm
			{
				bool IsNetpbm(std::string_view const& ext)
				{
					return ext == "ppm" || ext == "pgm" || ext == "pbm";
				}

				bool IsNetpbm(std::wstring_view const& ext)
				{
					return ext == L"ppm" || ext == L"pgm" || ext == L"pbm";
				}
			}

			namespace stbi
			{
				bool CanReadWrite(std::string_view const& ext)
				{
					return ext == "png" || ext == "jpg" || ext == "jpeg" || ext == "tga" || ext == "hdr";
				}

				bool CanReadWrite(std::wstring_view const& ext)
				{
					return ext == L"png" || ext == L"jpg" || ext == L"jpeg" || ext == L"tga" || ext == L"hdr";
				}
			}

			std::string ConvertWString(std::wstring_view const& wstr)
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
			that::PathStringView ExtractExtensionSV(const std::filesystem::path * ext_path)
			{
				that::PathString const& s = ext_path->native();
				assert(s.front() == '.');
				that::PathStringView ext = that::PathStringView(s.data() + 1, s.size() - 1);
				return ext;
			}
		}
}

}