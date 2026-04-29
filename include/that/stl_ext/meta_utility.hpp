#pragma once

#include <type_traits>

namespace std
{
	template <class ...Ts>
	struct pack_list final {};

	template <template <class T> class Transform, class ... Ts>
	struct transform_types
	{
		using types = pack_list<Transform<Ts>...>;
	};

	template <bool b = false, typename T = void>
	struct optional_type_identity
	{
		using type = T;
		using value_type = bool;
		static const constinit bool value = b;
	};

	namespace concepts
	{
		template <class ... Ts>
		concept has_common_type = requires
		{
			typename std::common_type<Ts...>::type;
		};
	}


	template <class ... Ts>
	struct optional_common_type : std::optional_type_identity<false> {};

	template <class ... Ts>
		requires concepts::has_common_type<Ts...>
	struct optional_common_type<Ts...> : std::optional_type_identity<true, typename std::common_type<Ts...>::type> {};

	template <class... Ts>
	struct optional_common_type<pack_list<Ts...>> : optional_common_type<Ts...> {};

	template <class... Ts>
	struct common_type<pack_list<Ts...>> : common_type<Ts...> {};
}