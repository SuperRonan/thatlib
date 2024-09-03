#pragma once

#include <concepts>
#include <bit>
#include <that/core/BasicTypes.hpp>
#include <that/stl_ext/alignment.hpp>

#define THAT_DECLARE_ENUM_CLASS_FLAG_OPERATOR(EnumClassName, op) \
static constexpr EnumClassName& operator ## op ## =(EnumClassName& l, EnumClassName r) noexcept\
{ \
	using ui_t = typename std::underlying_type<EnumClassName>::type; \
	ui_t u = static_cast<ui_t>(l); \
	u op ## = static_cast<ui_t>(r); \
	l = static_cast<EnumClassName>(u); \
	return l; \
} \
static constexpr EnumClassName operator ## op(EnumClassName l, EnumClassName r) noexcept\
{ \
	using ui_t = typename std::underlying_type<EnumClassName>::type; \
	return static_cast<EnumClassName>(static_cast<ui_t>(l) op static_cast<ui_t>(r)); \
}

#define THAT_DECLARE_ENUM_CLASS_FLAG_OPERATOR_INTEGRAL(EnumClassName, op) \
template <std::integral Int> \
static constexpr EnumClassName& operator ## op ## =(EnumClassName& l, Int r) noexcept\
{ \
	using ui_t = typename std::underlying_type<EnumClassName>::type; \
	ui_t u = static_cast<ui_t>(l); \
	u op ## = static_cast<ui_t>(r); \
	l = static_cast<EnumClassName>(u); \
	return l; \
} \
template <std::integral Int> \
static constexpr EnumClassName operator ## op(EnumClassName l, Int r) noexcept\
{ \
	using ui_t = typename std::underlying_type<EnumClassName>::type; \
	return static_cast<EnumClassName>(static_cast<ui_t>(l) op static_cast<ui_t>(r)); \
}

#define THAT_DECLARE_ENUM_CLASS_FLAG_BOOL_OPERATOR(EnumClassName) \
static constexpr bool operator!(EnumClassName e) \
{ \
	using ui_t = typename std::underlying_type<EnumClassName>::type; \
	const bool res = static_cast<ui_t>(e) == 0; \
	return res; \
} \
static constexpr EnumClassName operator~(EnumClassName e) \
{ \
	using ui_t = typename std::underlying_type<EnumClassName>::type; \
	const ui_t res = ~static_cast<ui_t>(e); \
	return static_cast<EnumClassName>(res); \
}

#define THAT_DECLARE_ENUM_CLASS_OPERATORS(EnumClassName) \
THAT_DECLARE_ENUM_CLASS_FLAG_OPERATOR(EnumClassName, |) \
THAT_DECLARE_ENUM_CLASS_FLAG_OPERATOR(EnumClassName, &) \
THAT_DECLARE_ENUM_CLASS_FLAG_OPERATOR(EnumClassName, ^) \
THAT_DECLARE_ENUM_CLASS_FLAG_OPERATOR(EnumClassName, <<) \
THAT_DECLARE_ENUM_CLASS_FLAG_OPERATOR(EnumClassName, >>) \
THAT_DECLARE_ENUM_CLASS_FLAG_OPERATOR(EnumClassName, +) \
THAT_DECLARE_ENUM_CLASS_FLAG_OPERATOR(EnumClassName, -) \
THAT_DECLARE_ENUM_CLASS_FLAG_OPERATOR_INTEGRAL(EnumClassName, |) \
THAT_DECLARE_ENUM_CLASS_FLAG_OPERATOR_INTEGRAL(EnumClassName, &) \
THAT_DECLARE_ENUM_CLASS_FLAG_OPERATOR_INTEGRAL(EnumClassName, ^) \
THAT_DECLARE_ENUM_CLASS_FLAG_OPERATOR_INTEGRAL(EnumClassName, <<) \
THAT_DECLARE_ENUM_CLASS_FLAG_OPERATOR_INTEGRAL(EnumClassName, >>) \
THAT_DECLARE_ENUM_CLASS_FLAG_OPERATOR_INTEGRAL(EnumClassName, +) \
THAT_DECLARE_ENUM_CLASS_FLAG_OPERATOR_INTEGRAL(EnumClassName, -) \
THAT_DECLARE_ENUM_CLASS_FLAG_BOOL_OPERATOR(EnumClassName)


namespace that
{
	namespace concepts
	{
		template <class Candidate>
		concept BitFieldAccessor = requires(Candidate c)
		{
			requires std::is_integral<decltype(c.value)>::value || std::is_enum<decltype(c.value)>::value;
			{Candidate::BitOffset} -> std::integral;
			{Candidate::BitCount} -> std::integral;
		};
	}
}


#define THAT_DECLARE_ENUM_CLASS_BIT_FIELD_OPERATOR(EnumClassName, ParentAccessorType, op) \
template <std::derived_from<ParentAccessorType> Accessor> \
static constexpr EnumClassName& operator ## op ## =(EnumClassName& f, Accessor a) \
{ \
	f op ## = (static_cast<EnumClassName>(a.value) << Accessor::BitOffset); \
	return f; \
} \
template <std::derived_from<ParentAccessorType> Accessor> \
static constexpr EnumClassName operator ## op (EnumClassName f, Accessor a) \
{ \
	f op ## = (static_cast<EnumClassName>(a.value) << Accessor::BitOffset); \
	return f; \
} 



#define THAT_DECLARE_ENUM_CLASS_BIT_FIELD_OPERATORS(EnumClassName, ParentAccessorType) \
THAT_DECLARE_ENUM_CLASS_BIT_FIELD_OPERATOR(EnumClassName, ParentAccessorType, |) \
THAT_DECLARE_ENUM_CLASS_BIT_FIELD_OPERATOR(EnumClassName, ParentAccessorType, &) \


