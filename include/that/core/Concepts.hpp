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

		template <template <class C> class Pred>
		concept UnaryTypePredictor = requires
		{
			{Pred<int>::value} -> std::convertible_to<bool>;
		};

		template <template <class L, class R> class Pred>
		concept BinaryTypePredictor = requires
		{
			{Pred<int, int>::value} -> std::convertible_to<bool>;
		};

		template <class T, template <class L, class R> class BinTypePred, class ... Args>
		concept AnyOf = (BinTypePred<T, Args>::value || ...);

		template <class T, template <class L, class R> class BinTypePred, class ... Args>
		concept AllOf = (BinTypePred<T, Args>::value && ...);

	}

	template <class T, template<class L, class R> class BinTypePred, class ... Args>
	struct FirstMatch;

	template <class T, template<class L, class R> class BinTypePred, class Head, class ... Tail>
	struct FirstMatch<T, BinTypePred, Head, Tail...> : public std::conditional<
		BinTypePred<T, Head>::value,
		Head,
		typename FirstMatch<T, BinTypePred, Tail...>::type
	> {};

	template <class T, template<class L, class R> class BinTypePred, class Head>
	struct FirstMatch<T, BinTypePred, Head> : public std::conditional<BinTypePred<T, Head>::value, Head, void> {};
}