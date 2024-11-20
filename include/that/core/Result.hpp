#pragma once

namespace that
{


	enum class Result	{
		Success,
		Error,
		Exception,
		Warning,
		NotImplemented,
		NotReady,
		Waiting,
		InvalidValue,
		InvalidParameter,
		IOError,
		FileWriteError,
		FileReadError,
		FileWriteException,
		FileReadException,
		CannotOpenFile,
		FileDoesNotExist,
		CannotConvertFormat,
		STBInteralError,
		WrongFileFormat,
		UnknownFileExtension,
		WrongFileMacro,
		UnknownFileMacro,
		UnknownMountingPoint,
		PathIsIllFormed,
		MAX_ENUM,
	};

	template <class T>
	struct ResultAnd
	{
		// Note: this is a generic solution, it might not provide the best memory layout.
		Result result = {};
		T value = {};
	};

	// Returns nullptr if the result code is unknown
	extern const char * GetResultStr(Result r);

	// Never returns nullptr
	extern const char * GetResultStrSafe(Result r);
}