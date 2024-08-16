#pragma once

#include <concepts>
#include <bit>

#define THAT_DECLARE_ENUM_CLASS_FLAG_OPERATOR(EnumClassName, EnumClassUint, op) \
static constexpr EnumClassName& operator ## op ## =(EnumClassName& l, EnumClassName r) noexcept\
{ \
	EnumClassUint u = static_cast<EnumClassUint>(l); \
	u op ## = static_cast<EnumClassUint>(r); \
	l = EnumClassName(u); \
	return l; \
} \
static constexpr EnumClassName operator ## op(EnumClassName l, EnumClassName r) noexcept\
{ \
	return static_cast<EnumClassName>(static_cast<EnumClassUint>(l) op static_cast<EnumClassUint>(r)); \
}

#define THAT_DECLARE_ENUM_CLASS_FLAG_OPERATOR_INTEGRAL(EnumClassName, EnumClassUint, op) \
template <std::integral Int> \
static constexpr EnumClassName& operator ## op ## =(EnumClassName& l, Int r) noexcept\
{ \
	EnumClassUint u = static_cast<EnumClassUint>(l); \
	u op ## = static_cast<EnumClassUint>(r); \
	l = EnumClassName(u); \
	return l; \
} \
template <std::integral Int> \
static constexpr EnumClassName operator ## op(EnumClassName l, Int r) noexcept\
{ \
	return static_cast<EnumClassName>(static_cast<EnumClassUint>(l) op static_cast<EnumClassUint>(r)); \
}

#define THAT_DECLARE_ENUM_CLASS_FLAG_BOOL_OPERATOR(EnumClassName, EnumClassUint) \
static constexpr bool operator!(EnumClassName e) \
{ \
	const bool res = static_cast<EnumClassUint>(e) == 0; \
	return res; \
}

#define THAT_DECLARE_ENUM_CLASS_OPERATORS(EnumClassName, EnumClassUint) \
THAT_DECLARE_ENUM_CLASS_FLAG_OPERATOR(EnumClassName, EnumClassUint, |) \
THAT_DECLARE_ENUM_CLASS_FLAG_OPERATOR(EnumClassName, EnumClassUint, &) \
THAT_DECLARE_ENUM_CLASS_FLAG_OPERATOR(EnumClassName, EnumClassUint, ^) \
THAT_DECLARE_ENUM_CLASS_FLAG_OPERATOR(EnumClassName, EnumClassUint, <<) \
THAT_DECLARE_ENUM_CLASS_FLAG_OPERATOR(EnumClassName, EnumClassUint, >>) \
THAT_DECLARE_ENUM_CLASS_FLAG_OPERATOR_INTEGRAL(EnumClassName, EnumClassUint, <<) \
THAT_DECLARE_ENUM_CLASS_FLAG_OPERATOR_INTEGRAL(EnumClassName, EnumClassUint, >>) \
THAT_DECLARE_ENUM_CLASS_FLAG_BOOL_OPERATOR(EnumClassName, EnumClassUint)

