#pragma once

#include <filesystem>
#include <span>
#include <unordered_set>
#include <chrono>
#include <map>

#include <that/IO/MountingPoints.hpp>
#include <that/utils/StringSet.hpp>
#include <that/core/Result.hpp>
#include <that/core/Core.hpp>
#include <that/utils/EnumClassOperators.hpp>

namespace that
{

#define PATH_MACRO_OPENING "$("
#define PATH_MACRO_CLOSING ")"
#define PATH_MACRO(LITERAL) PATH_MACRO_OPENING LITERAL PATH_MACRO_CLOSING

#if _WINDOWS
#define PATH_LITERAL(LITERAL) FORWARD_MACRO(L) ## FORWARD_MACRO(LITERAL)
#else
#define PATH_LITERAL(LITERAL) FORWARD_MACRO(LITERAL)
#endif

	class FileSystem
	{
	public:

		using Path = std::filesystem::path;
		using PathString = Path::string_type;
		using PathChar = Path::value_type;
		using PathStringView = std::basic_string_view<PathChar>;
		using Clock = std::chrono::file_clock;
		using TimePoint = Clock::time_point;
		using Duration = Clock::duration;

		enum class Hint : uint32_t
		{
			None = 0x0,
			PathIsNative = 0x1,
			PathIsCannon = PathIsNative << 1,
			DontCreateDirectory = PathIsCannon << 1,
			UnknownMacroAsError = 0x0,
			UnknownMacroAsBlank = DontCreateDirectory << 1,
			UnknownMacroAsItSelf = UnknownMacroAsBlank << 1,
			UnknownMacroUnmodified = UnknownMacroAsBlank | UnknownMacroAsItSelf,
			UnknownMacroMask = UnknownMacroUnmodified,
		};

		using MacroMap = StringMap<PathChar, PathString>;

		static constexpr PathStringView GetMacroOpening()
		{
			using namespace std::string_literals;
			return PathStringView(PATH_LITERAL(PATH_MACRO_OPENING));
		}

		static constexpr PathStringView GetMacroClosing()
		{
			return PathStringView(PATH_LITERAL(PATH_MACRO_CLOSING));
		}

	protected:
	
		MountingPoints _mounting_points = {};

		MacroMap _macros = {};

		// Could be static, maybe, but FileSystem itself might static anyway (or at least a singleton)
		StringSet<PathChar> _system_reserved_mounting_points = {};

		void querySystemMountingPoints();

	public:

		struct CreateInfo
		{
			MountingPoints _mounting_points = {};
		};
		using CI = CreateInfo;

		FileSystem(CreateInfo const& ci);

		~FileSystem();

		MountingPoints& mountingPoints()
		{
			return _mounting_points;
		}

		MountingPoints const& mountingPoints() const
		{
			return _mounting_points;
		}

		MacroMap& macros()
		{
			return _macros;
		}

		MacroMap const& macros() const
		{
			return _macros;
		}

		// Extracts the mounting point of path (what is left of :) 
		// "ShaderLib:/folder/shader.glsl" -> "ShaderLib"
		// "C:/folder/folder2/" -> "C"
		static ResultAnd<PathStringView> ExtractMountingPoint(PathStringView const& path);

		static ResultAnd<PathStringView> ExtractMountingPoint(Path const& path)
		{
			return ExtractMountingPoint(PathStringView(path.native()));
		}

		// Extracts the path right of the mounting point (if exists)
		// "ShaderLib:/folder/shader.glsl" -> "folder/shader.glsl"
		// "C:/folder/folder2/" -> "folder/folder2/"
		static ResultAnd<PathStringView> ExtractRelative(PathStringView const& path);

		static ResultAnd<PathStringView> ExtractRelative(Path const& path)
		{
			return ExtractRelative(PathStringView(path.native()));
		}

		// On Windows
		bool mountingPointIsNative(PathStringView const& mp) const;

		bool knowsMountingPoint(PathStringView const& mp) const;

		ResultAnd<Path> resolveMountingPointsIFN(PathStringView const& path) const;
		ResultAnd<Path> resolveMountingPointsIFN(Path const& path) const
		{
			return resolveMountingPointsIFN(PathStringView(path.native()));
		}

		ResultAnd<Path> resolveMacros(PathStringView const& path, Hint hint = Hint::None) const;

		ResultAnd<Path> resolve(PathStringView const& path, Hint hint = Hint::None) const;

		ResultAnd<Path> resolve(Path const& path, Hint hint = Hint::None) const
		{
			return resolve(PathStringView(path.native()), hint);
		}

		ResultAnd<Path> cannonize(Path const& path) const;

		struct ReadFileInfo
		{
			Hint hint = Hint::None;
			const Path * path = nullptr;
			std::vector<uint8_t> * result_vector = nullptr;
			std::string * result_string = nullptr;
		};
		Result readFile(ReadFileInfo const& info);

		static Result ReadFileFromDisk(ReadFileInfo const& info);
		static Result ReadFile(ReadFileInfo const& info, FileSystem * fs = nullptr);

		struct WriteFileInfo
		{
			Hint hint = Hint::None;
			const Path * path = nullptr;
			std::span<const uint8_t> data = {};
		};
		Result writeFile(WriteFileInfo const& info);

		static Result WriteFileToDisk(WriteFileInfo const& info);
		static Result WriteFile(WriteFileInfo const& info, FileSystem * fs = nullptr);

		ResultAnd<TimePoint> getFileLastWriteTime(Path const& path, Hint hint = Hint::None) const;

		Result checkFileExists(Path const& path, Hint hint = Hint::None) const;

		struct FileInfos
		{
			Result result = {};
			TimePoint last_write_time = {};
			std::filesystem::file_status status = {};
		};

		FileInfos getFileInfos(Path const& path, Hint hint, bool get_write_time, bool get_status) const;
		FileInfos getFileInfos(Path const& path, Hint hint = Hint::None) const
		{
			return getFileInfos(path, hint, true, true);
		}

		// TODO get a handle to a file 
	};

	THAT_DECLARE_ENUM_CLASS_OPERATORS(FileSystem::Hint);
}