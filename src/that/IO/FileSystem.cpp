#include <that/IO/FileSystem.hpp>

#include <that/IO/File.hpp>

#include <Windows.h>

namespace that
{
	FileSystem::FileSystem(CreateInfo const& ci):
		_mounting_points(ci._mounting_points)
	{
		querySystemMountingPoints();
	}

	void FileSystem::querySystemMountingPoints()
	{
#if _WINDOWS
		_system_reserved_mounting_points.clear();
		std::vector<PathString> drives;
		DWORD N = GetLogicalDriveStringsW(0, nullptr);
		std::vector<WCHAR> buffer(N + 16); // +1 to include the final null terminator (+16 to be safe)
		GetLogicalDriveStringsW(N, buffer.data());
		size_t it = 0;
		while(it < N && buffer[it])
		{
			PathStringView drive_name = (buffer.data() + it);
			if (!drive_name.empty())
			{
				size_t sep = drive_name.find(PathChar(':'));
				if (sep != PathString::npos)
				{
					PathStringView mp_name(buffer.data() + it, sep);
					_system_reserved_mounting_points.insert(PathString(mp_name));
				}
				it += (drive_name.size() + 1);
			}
			else
			{
				break;
			}
		}
#endif
	}

	FileSystem::~FileSystem()
	{}

	ResultAnd<FileSystem::PathStringView> FileSystem::ExtractMountingPoint(PathStringView const& path)
	{
		PathStringView res;
		const size_t it = path.find(PathChar(':'));
		if (it != PathString::npos)
		{
			res = PathStringView(path.data(), it);
		}
		return { Result::Success, res };
	}

	ResultAnd<FileSystem::PathStringView> FileSystem::ExtractRelative(PathStringView const& path)
	{
		PathStringView res = path;
		const size_t it = path.find(PathChar(':'));
		if (it != PathString::npos)
		{
			size_t s = 2;
			while ((it + s) < path.size())
			{
				const PathChar & c = path[it + s];
				if (c == PathChar('/') || c == PathChar('\\'))
				{
					++s;
				}
				else
				{
					break;
				}
			}
			res = PathStringView(path.data() + (it + s), path.size() - (it + s));
		}
		return { Result::Success, res };
	}
	
	bool FileSystem::mountingPointIsNative(PathStringView const& mp) const
	{
		return _system_reserved_mounting_points.contains(mp);
	}


	bool FileSystem::knowsMountingPoint(PathStringView const& mp) const
	{
		bool res = false;
		res = _mounting_points.contains(mp);
		return res;
	}

	ResultAnd<FileSystem::Path> FileSystem::resolveMountingPointsIFN(PathStringView const& path) const
	{
		ResultAnd<PathStringView> mpr = ExtractMountingPoint(path);
		PathStringView & mp = mpr.value;
		Path res;
		if (mpr.result != Result::Success)
		{
			return {mpr.result, {}};
		}
		if (mp.empty() || mountingPointIsNative(mp) || !knowsMountingPoint(mp))
		{
			res = path;
		}
		else
		{
			ResultAnd<PathStringView> rel = ExtractRelative(path);
			if (rel.result == Result::Success)
			{
				res = _mounting_points.at(mp) / rel.value;
			}
			else
			{
				return {rel.result, {}};
			}
		}
		return { Result::Success, res };
	}

	ResultAnd<FileSystem::Path> FileSystem::resolveMacros(PathStringView const& path, Hint hint) const
	{
		// TODO nested macro resolution
		// For example: if macro A is known as "$(B)", then we need to solve $(B) (and so one recursively / iteratively)
		// What about $($(C))? Should it be legal (I think yes, but it requires more complex parsing)
		PathString res;
		size_t copied_so_far = 0;
		const PathStringView op = GetMacroOpening(), cl = GetMacroClosing();

		auto absorbUpTo = [&](size_t index)
		{
			res += PathStringView(path.data() + copied_so_far, index - copied_so_far);
			copied_so_far = index;
		};

		hint = hint & Hint::UnknownMacroMask;

		while (copied_so_far < path.size())
		{
			size_t opening = path.find(op, copied_so_far);
			if (opening != PathString::npos)
			{
				size_t closing = path.find(cl, opening + op.size());
				if (closing == PathString::npos)
				{
					return {Result::WrongFileMacro, {}};
				}
				else
				{
					absorbUpTo(opening);

					const PathStringView macro = PathStringView(path.data() + opening + op.size(), closing - (opening + op.size()));
					if (_macros.contains(macro))
					{
						res += _macros.at(macro);
					}
					else
					{
						if (hint == Hint::UnknownMacroAsBlank)
						{}
						else if (hint == Hint::UnknownMacroAsItSelf)
						{
							res += macro;
						}
						else if (hint == Hint::UnknownMacroUnmodified)
						{
							res += op;
							res += macro;
							res += cl;
						}
						else
						{
							return {Result::UnknownFileMacro, {}};
						}
					}

					copied_so_far = closing + cl.size();
				}
			}
			else
			{
				break;
			}
		}
		if (copied_so_far != path.size())
		{
			absorbUpTo(path.size());
		}
		return {Result::Success, res};
	}

	ResultAnd<FileSystem::Path> FileSystem::resolve(PathStringView const& path, Hint hint) const
	{
		ResultAnd<Path> res = resolveMacros(path, hint);
		if (res.result == Result::Success)
		{
			res = resolveMountingPointsIFN(res.value);
		}
		return res;
	}

	ResultAnd<FileSystem::Path> FileSystem::cannonize(Path const& path) const
	{
		std::error_code ec;
		ResultAnd<FileSystem::Path> res;
		res.value = std::filesystem::canonical(path, ec);
		if (ec)
		{
			res.result = Result::InvalidParameter;
		}
		return res;
	}
	
	static thread_local ResultAnd<FileSystem::Path> _tmp_path;

	Result FileSystem::readFile(ReadFileInfo const& info)
	{
		Result res = Result::Success;
		const Path * path;
		if (info.path)
		{
			if (!!(info.hint & Hint::PathIsNative))
			{
				path = info.path;
			}
			else 
			{
				_tmp_path = resolve(*info.path);
				if (_tmp_path.result != Result::Success)
				{
					return _tmp_path.result;
				}
				path = &_tmp_path.value;	
			}

			ReadFileInfo info2 = info;
			info2.path = path;
			res = ReadFileFromDisk(info2);
		}
		else
		{
			res = Result::InvalidParameter;
		}
		return res;
	}

	Result FileSystem::ReadFileFromDisk(ReadFileInfo const& info)
	{
		Result res = Result::Success;
		if (!info.path)
		{
			res = Result::InvalidParameter;
		}
		else
		{
			if (info.result_vector)
			{
				res = ::that::ReadFile(*info.path, *info.result_vector);
			}
			else if (info.result_string)
			{
				res = ::that::ReadFileToString(*info.path, *info.result_string);
			}
			else
			{
				res = Result::InvalidParameter;
			}
		}
		return res;
	}

	Result FileSystem::ReadFile(ReadFileInfo const& info, FileSystem* fs)
	{
		if (fs)
		{
			return fs->readFile(info);
		}
		else
		{
			return FileSystem::ReadFileFromDisk(info);
		}
	}

	Result FileSystem::writeFile(WriteFileInfo const& info)
	{
		Result res = Result::Success;
		const Path* path;
		if (info.path)
		{
			if (!!(info.hint & Hint::PathIsNative))
			{
				path = info.path;
			}
			else
			{
				_tmp_path = resolve(*info.path);
				path = &_tmp_path.value;
			}

			WriteFileInfo info2 = info;
			info2.path = path;

			res = FileSystem::WriteFileToDisk(info2);
		}
		else
		{
			res = Result::InvalidParameter;
		}
		return res;
	}

	Result FileSystem::WriteFileToDisk(WriteFileInfo const& info)
	{
		Result res = Result::Success;
		if (!info.path)
		{
			res = Result::InvalidParameter;
		}
		else
		{
			if (!(info.hint & Hint::DontCreateDirectory))
			{
				res = CreateDirectoriesIFP(info.path->parent_path());
			}
			if (res == Result::Success)
			{
				res = ::that::WriteFile(*info.path, info.data);
			}
		}
		return res;
	}

	Result FileSystem::WriteFile(WriteFileInfo const& info, FileSystem* fs)
	{
		if (fs)
		{
			return fs->writeFile(info);
		}
		else
		{
			return FileSystem::WriteFileToDisk(info);
		}
	}

	ResultAnd<FileSystem::TimePoint> FileSystem::getFileLastWriteTime(Path const& path, Hint hint) const
	{
		FileInfos const& infos = getFileInfos(path, hint);
		return {infos.result, infos.last_write_time};
	}

	FileSystem::FileInfos FileSystem::getFileInfos(Path const& path, Hint hint, bool get_file_time, bool get_status) const
	{
		if (!(hint & Hint::PathIsNative))
		{
			_tmp_path = resolve(path);
			if (_tmp_path.result == Result::Success)
			{
				return getFileInfos(_tmp_path.value, hint | Hint::PathIsNative);
			}
			else
			{
				FileInfos res;
				res.result = _tmp_path.result;
				return res;
			}
		}
		else
		{
			FileInfos res;
			if (std::filesystem::exists(path))
			{
				res.result = Result::Success;
				if (get_file_time)
				{
					res.last_write_time = std::filesystem::last_write_time(path);
				}
				if (get_status)
				{
					res.status = std::filesystem::status(path);
				}
			}
			else
			{
				res.result = Result::FileDoesNotExist;
			}
			return res;
		}
	}

	Result FileSystem::checkFileExists(Path const& path, Hint hint) const
	{
		return getFileInfos(path, hint, false, false).result;
	}
}