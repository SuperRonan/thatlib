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

		enum class Hint
		{
			None = 0x0,
			DontCreateDirectory = 0x1,
			UnknownMacroAsError = 0x0,
			UnknownMacroAsBlank = 0x2,
			UnknownMacroAsItSelf = 0x4,
			UnknownMacroUnmodified = 0x6,
			UnknownMacroMask = 0x6,
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

		ResultAnd<Path> resolve(PathStringView const& path, Hint hint = Hint::None) const
		{
			ResultAnd<Path> res = resolveMacros(path, hint);
			if (res.result == Result::Success)
			{
				res = resolveMountingPointsIFN(res.value);
			}
			return res;
		}

		ResultAnd<Path> resolve(Path const& path, Hint hint = Hint::None) const
		{
			return resolve(PathStringView(path.native()), hint);
		}

		struct ReadFileInfo
		{
			Hint hint = Hint::None;
			bool path_is_native = false;
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
			bool path_is_native = false;
			const Path * path = nullptr;
			std::span<const uint8_t> data = {};
		};
		Result writeFile(WriteFileInfo const& info);

		static Result WriteFileToDisk(WriteFileInfo const& info);
		static Result WriteFile(WriteFileInfo const& info, FileSystem * fs = nullptr);

		ResultAnd<TimePoint> getFileTime(Path const& path, bool path_is_native = false) const;

		// TODO get a handle to a file 
	};

	THAT_DECLARE_ENUM_CLASS_OPERATORS(FileSystem::Hint);
}