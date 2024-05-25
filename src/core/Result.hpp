#pragma once

namespace that
{
	enum class Result
	{
		Success,
		Error,
		Exception,
		Warning,
		NotImplemented,
		NotReady,
		Waiting,
		InvalidValue,
		IOError,
		FileWriteError,
		FileReadError,
		FileWriteException,
		FileReadException,
		CannotConvertFormat,
		STBInteralError,
		WrongFileFormat,
		MAX_ENUM,
	};

	extern const char * GetResultStr(Result r);

	extern const char * GetResultStrSafe(Result r);
}