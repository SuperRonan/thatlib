#pragma once

#include <type_traits>
#include <concepts>

namespace that
{
	namespace concepts
	{
		template <class Candidate, class Type>
		concept UniversalReference = 
			(std::is_rvalue_reference<Candidate>::value || std::is_lvalue_reference<Candidate>::value) &&
			(std::is_same<typename std::remove_cvref<Candidate>::type, Type>::value);
	}
}