#pragma once

#include <filesystem>
#include <that/utils/StringMap.hpp>

namespace that
{
	using MountingPoints = StringMap<std::filesystem::path::value_type, std::filesystem::path>;
}