#include <that/utils/PointerDynamicVariant.hpp>

#include <that/stl_ext/visitor.hpp>

namespace that
{
	class Base
	{
		int b;

	public:
		virtual ~Base() = default;
	};

	class Base2
	{
		float f;
	public:
		virtual ~Base2() = default;
	};

	class D1 : public Base
	{

	};

	class D2 : public Base
	{

	};

	class D3 : public Base
	{

	};

	class D4 : public Base, public Base2
	{

	};

	class D1D1 : public D1
	{

	};

	class D3D1 : public D3
	{

	};

	using D3Ref = PointerDynamicVariant<RawPointer, Base, D3>;
	using D1Ref = PointerDynamicVariant<RawPointer, Base, D1>;
	using D2Ref = PointerDynamicVariant<RawPointer, Base, D1>;
	using D12Ref = PointerDynamicVariant<RawPointer, Base, D1, D2>;
	using D12SPtr = PointerDynamicVariant<std::shared_ptr, Base, D1, D2>;

	template <class Derived, class Base>
	concept PointerCompatible = std::is_pointer_interconvertible_base_of<Base, Derived>::value;
	
	//static_assert(PointerCompatible<D1, Base> == true);
	//static_assert(PointerCompatible<D1D1, Base> == true);
	//static_assert(PointerCompatible<D4, Base2> == false);
	//static_assert(PointerCompatible<D4, Base> == true);
	static_assert(std::is_base_of<Base, D1>::value == true);

	void f(Base* b, D12Ref d, D3Ref d3, std::shared_ptr<D1D1> const& d1d1, D1Ref d1)
	{
		std::pointer_traits<decltype(d1d1)>;
		static_cast<D1*>(b);
		reinterpret_cast<D1*&>(b);

		d.index();
		d3.index();
		d.getRaw();

		D12Ref d12(static_cast<D1*>(b));
		D12SPtr sd_(d1d1);
		sd_.getRaw();
		D12Ref sd_2 = d1;

		sd_2 = sd_.getRawVariant();

		const D12Ref & my_ref = D12Ref::ReinterpretAsVariant(reinterpret_cast<D1*&>(b));

		constexpr bool t3 = D3Ref::TypeIsCompatible<D3>();
		constexpr bool t31 = D3Ref::TypeIsCompatible<D3D1>();

		auto visitor = std::overloads
		{
			[](D1 const&) {},
			[](D2 const&) {},
			[](nullptr_t) {}
		};
		//std::visitor_result<decltype(visitor), D1, D2>::type;
		my_ref.visit(visitor);

	}

	
}

