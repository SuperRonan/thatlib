#include <that/stl_ext/visitor.hpp>

#include <variant>
#include <tuple>

#include <that/core/Concepts.hpp>

using Vfi = std::variant<int, float>;



void test()
{
	const auto visitor = std::overloads
	{
		[](int i){return i; },
		[](float f){return f;},
	};

	Vfi v = {};
	//std::visit(visitor, v);

	using VisRetTypes = std::impl::visitor_result_type_list<decltype(visitor), int, float>::types;
	//std::impl::optional_common_type<VisRetTypes>::

	static_assert(std::concepts::visitor<decltype(visitor), int, float>);

	std::common_type<void, void>::type;

	std::visitor_result<decltype(visitor), int, float>::type;

	//using Pointers = TransformTypes<std::add_pointer_t, int, float>;

	{
		using Trait = std::optional_common_type<float, int, int*>;
		using T = Trait::type;
		constexpr const bool B = Trait::value;
		static_assert(B == false);
		static_assert(std::same_as<T, void>);
	}

	{
		using Trait = std::optional_common_type<float, int, double>;
		using T = Trait::type;
		constexpr const bool B = Trait::value;
		static_assert(B == true);
		static_assert(std::same_as<T, double>);
	}
}