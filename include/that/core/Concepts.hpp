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
		concept AnyOfBinary = (BinTypePred<T, Args>::value || ...);

		template <class T, template <class L, class R> class BinTypePred, class ... Args>
		concept AllOfBinary = (BinTypePred<T, Args>::value && ...);

		template <template <class T> class UnaryTypePred, class ... Args>
		concept AllOfUnary = (UnaryTypePred<Args>::value && ...);

		template <template <class T> class UnaryTypePred, class ... Args>
		concept AnyOfUnary = (UnaryTypePred<Args>::value || ...);

	}

	template <template <class T> class UnaryTypePred, class ...Args>
	struct FirstMatch1;

	template <template <class T> class UnaryTypePred, class Head, class ...Tail>
	struct FirstMatch1<UnaryTypePred, Head, Tail...> : std::conditional<
		UnaryTypePred<Head>::value,
		Head,
		typename FirstMatch1<UnaryTypePred, Tail...>::type
	>{};

	template <template <class T> class UnaryTypePred, class Tail>
	struct FirstMatch1<UnaryTypePred, Tail> : std::conditional<UnaryTypePred<Tail>::value, Tail, void> {};

	template <class T, template <class L, class R> class BinTypePred>
	struct UnaryTypePredFromBinary
	{
		template <class RHS>
		using PredL = BinTypePred<T, RHS>;

		template <class LHS>
		using PredR = BinTypePred<LHS, T>;
	};

	template <class T, template<class L, class R> class BinTypePred, class ... Args>
	using FirstMatch2 = FirstMatch1<UnaryTypePredFromBinary<T, BinTypePred>::template PredL, Args...>;

	namespace concepts
	{
		template <class E>
		concept Enumeration = std::is_enum<E>::value;
	}
}