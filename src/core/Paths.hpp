#pragma once

#include "Strings.hpp"
#include <filesystem>

namespace that
{
	using PathString = std::filesystem::path::string_type;

	using PathStringView = std::basic_string_view<std::filesystem::path::value_type>;
}