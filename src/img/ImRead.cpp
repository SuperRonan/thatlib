#include "ImRead.hpp"

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
	}
}