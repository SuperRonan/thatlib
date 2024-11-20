#include <that/core/Result.hpp>

#include <array>

namespace that
{
	std::array result_table = {
		"Success",
		"Error",
		"Exception",
		"Warning",
		"NotImplemented",
		"NotReady",
		"Waiting",
		"InvalidValue",
		"InvalidParameter",
		"IOError",
		"FileWriteError",
		"FileReadError",
		"FileWriteException",
		"FileReadException",
		"CannotOpenFile",
		"FileDoesNotExist",
		"CannotConvertFormat",
		"STBInteralError",
		"WrongFileFormat",
		"UnknownFileExtension",
		"WrongFileMacro",
		"UnknownFileMacro",
		"UnknownMountingPoint",
		"PathIsIllFormed",
		"MAX_ENUM",
	};

	const char* GetResultStr(Result r)
	{
		const int i = static_cast<int>(r);
		const char * res = nullptr;
		if (i < result_table.size())
		{
			res = result_table[i];
		}
		return res;
	}

	const char* GetResultStrSafe(Result r)
	{
		const char * res = GetResultStr(r);
		if (!res)
		{
			res = "Unknown Result";
		}
		return res;
	}
}