#include "ImWrite.h"

namespace img
{
	namespace io
	{
	
		bool writeFile(byte* data, size_t size, const wchar_t* path)
		{
			FILE* file;
			errno_t error = _wfopen_s(&file, path, L"wb");
			if (file && error == 0)
			{
				fwrite(data, sizeof byte, size, file);
				fclose(file);
				return true;
			}
			throw std::runtime_error(std::string("Could not open the file ") + convertWString(path));
			return false;
		}

		void stbi::writeFileCallback(void* context, void* data, int len)
		{
			stbi::WriteContext* _context = (stbi::WriteContext *) context;
			bool res = writeFile((byte*)data, len, _context->path);
			assert(res);
		}
	
	}



}