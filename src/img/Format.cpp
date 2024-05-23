#include <img/Format.hpp>

namespace that
{
	static const char* s_element_type_names[] = {
		"UNORM",
		"SNORM",
		"UINT",
		"SINT",
		"sRGB",
		"FLOAT",
		"UNKNOWN",
	};

	static const char* GetElementTypeName(ElementType t)
	{
		const char * res = nullptr;
		const uint32_t i = static_cast<uint32_t>(t);
		if (i < sizeof(s_element_type_names) / sizeof(const char *))
		{
			res = s_element_type_names[i];
		}
		return res;
	}
}