#pragma once

#include <concepts>
#include <that/stl_ext/meta_utility.hpp>

namespace std
{
	namespace impl
	{
		template <class Visitor>
		struct visitor_result_helper
		{
			template <class T>
			using With = std::invoke_result_t<Visitor, T>;
		};

		template <class Visitor, class... Ts>
		struct visitor_result_type_list : transform_types<typename visitor_result_helper<Visitor>::With, Ts...> {};

		// 
		template <class Visitor, class... Ts>
		struct impl_visitor_result : std::common_type<typename visitor_result_type_list<Visitor, Ts...>::types> {};


		template <class Visitor, class... Ts>
		struct visitor_optional_result_type : optional_common_type<typename visitor_result_type_list<Visitor, Ts...>::types> {};
	}

	namespace concepts
	{
		template <class V, class... Ts>
		concept visitor = (std::invocable<V, Ts&&> && ...) && impl::visitor_optional_result_type<V, Ts...>::value;
	}

	template <class Visitor, class... Ts>
		requires concepts::visitor<Visitor, Ts...>
	using visitor_result = impl::impl_visitor_result<Visitor, Ts...>;

	template <class Visitor, class... Ts>
		requires concepts::visitor<Visitor, Ts...>
	using visitor_result_t = typename visitor_result<Visitor, Ts...>::type;


	// helper type for lambda visitor
	// Courtesy of https://en.cppreference.com/cpp/utility/variant/visit#:~:text=member%20visit-,Example,-Run%20this%20code
	template<class... Ts>
	struct overloads : Ts... { using Ts::operator()...; };
}
