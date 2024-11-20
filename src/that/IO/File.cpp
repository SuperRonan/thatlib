#include <that/IO/File.hpp>

#include <fstream>
#include <thread>
#include <chrono>

#if _WINDOWS
#include <Windows.h>
#endif

namespace that
{
	Result ReadFile(std::filesystem::path const& path, std::vector<uint8_t> & res)
	{
		Result result = Result::Success;
		if (std::filesystem::exists(path))
		{
			std::ifstream file(path, std::ios::ate | std::ios::binary);
			if (!file.is_open())
			{
				result = Result::CannotOpenFile;
				return result;
			}

			size_t size = file.tellg();
			res.resize(size);
			file.seekg(0);
			file.read((char*)res.data(), size);
			file.close();
		}
		else
		{
			result = Result::FileDoesNotExist;
		}
		return result;
	}

	Result ReadFileToString(std::filesystem::path const& path, std::string & res)
	{
		Result result = Result::Success;
		if (std::filesystem::exists(path))
		{
			std::ifstream file;
			int tries = 0;
			const int max_tries = 1;
			do
			{
				if (tries)
				{
					using namespace std::chrono_literals;
					std::this_thread::sleep_for(1us);
				}
				file = std::ifstream(path, std::ios::ate | std::ios::binary);
				++tries;
			} while (!file.is_open() && tries != max_tries);

			if (!file.is_open())
			{
				result = Result::CannotOpenFile;
				return result;
			}

			size_t size = file.tellg();
			res.resize(size);
			file.seekg(0);
			file.read((char*)res.data(), size);
			file.close();
		}
		else
		{
			result = Result::FileDoesNotExist;
		}
		return result;
	}

	Result WriteFile(std::filesystem::path const& path, std::span<const uint8_t> data)
	{
		Result result = Result::Success;
		std::ofstream file(path, std::ios::ate | std::ios::binary);
		if (!file.is_open())
		{
			result = Result::CannotOpenFile;
			return result;
		}

		file.write((const char*)data.data(), data.size_bytes());
		file.close();
		return result;
	}

	std::filesystem::path GetCurrentExecutableAbsolutePath()
	{
		std::filesystem::path res;
#if _WINDOWS
		const DWORD cap = 1024;
		CHAR win_path[cap];
		DWORD _res = GetModuleFileName(NULL, win_path, cap);
		if (_res > 0)
		{
			res = win_path;
		}
#endif
		return std::filesystem::absolute(res);
	}

	Result CreateDirectoriesIFP(std::filesystem::path const& folder_path)
	{
		std::error_code ec = {};
		bool created = std::filesystem::create_directories(folder_path, ec);
		Result res = Result::Success;
		if (ec.value() != 0)
		{
			// TODO
			res = Result::IOError;
		}
		return res;
	}
}