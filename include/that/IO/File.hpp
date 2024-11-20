#pragma once

#include <filesystem>
#include <span>

#include <that/core/Result.hpp>

namespace that
{
	Result ReadFile(std::filesystem::path const& path, std::vector<uint8_t> & res);

	Result ReadFileToString(std::filesystem::path const& path, std::string & res);

	Result WriteFile(std::filesystem::path const& path, std::span<const uint8_t> data);

	std::filesystem::path GetCurrentExecutableAbsolutePath();

	Result CreateDirectoriesIFP(std::filesystem::path const& folder_path);
}