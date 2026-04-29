#pragma once

#include <type_traits>
#include <concepts>
#include <utility>
#include <memory>
#include <algorithm>

#include <that/stl_ext/pointer.hpp>
#include <that/stl_ext/visitor.hpp>
#include <that/core/BasicTypes.hpp>
#include <that/core/Concepts.hpp>

namespace std
{
	template <class T>
	concept polymorphic_type = std::is_polymorphic<T>::value;

	template <class Derived, class Base>
	struct is_derived_from final : public std::bool_constant<std::derived_from<Derived, Base>>{};

	template <class Derived, class Base>
	constexpr bool is_derived_from_v = derived_from<Derived, Base>;

}

namespace that
{
	template <class T>
	using RawPointer = T*;

	namespace impl
	{
		using VariantIndex = uint;

		template <class T, class... Derived>
		concept TypeInVariant = std::polymorphic_type<T> && concepts::AnyOfBinary<T, std::is_derived_from, Derived...>;

		template <class T, class... Derived>
		struct TypeIsInVariant final : public ::std::bool_constant<TypeInVariant<T, Derived...>> {};

		template <class Type, class Base>
		static constexpr bool CheckType(Base* ptr)
		{
			return dynamic_cast<Type*>(ptr) != nullptr;
		}

		//template <class Index, class Base, class... T>
		//static constexpr Index TestIndex(Base* ptr, Index i = 0);

		// Assume ptr is not nullptr, and is of a type in the pack
		template <class Head, class ...Tail, class Base, class Index=VariantIndex>
		static constexpr Index TestIndex(Base* ptr, Index i = 0)
		{
			if constexpr (sizeof...(Tail) == 0)
			{
				assert(CheckType<Head>(ptr));
				return i;
			}
			else
			{
				if (dynamic_cast<Head*>(ptr))
				{
					return i;
				}
				else
				{
					return TestIndex<Tail...>(ptr, i + 1);
				}
			}
		}

		// Assumes this is not null
		template <class Res, class Head, class ...Tail, class Visitor, class Base>
			//requires std::concepts::visitor<Head, Tail...> && std::convertible_to<typename std::visitor_result<Visitor, Head, Tail...>::type, Res>
		static constexpr typename Res TestVisitor(Visitor&& vis, Base* ptr)
		{
			assert(ptr != nullptr);
			if constexpr (sizeof...(Tail) == 0)
			{
				assert(impl::CheckType<Head>(ptr));
				return vis(*static_cast<Head*>(ptr));
			}
			else
			{
				if (Head* h = dynamic_cast<Head*>(ptr))
				{
					return vis(*h);
				}
				else
				{
					return TestVisitor<Res, Tail...>(std::forward<Visitor>(vis), ptr);
				}
			}
		}
	}
	// Pointer wrapper, relying on polymorphism to deduce which Derived is set
	// _Pointer may be a raw pointer (use the above RawPointer, not std::add_pointer_t), or a smart pointer such as std::shared_ptr, std::weak_pointer, std::unique_ptr, ...
	// _Base cannot be deduced yet (maybe doable with reflection / C++26)
	// Each Derived type in the pack should be unique and independant
	template <template <class> class _Pointer, std::polymorphic_type _Base, std::derived_from<_Base>... Derived>
		//requires (sizeof...(Derived) >= 1)
	class PointerDynamicVariant
	{
	public:

		using Base = _Base;
		using MyPointer = _Pointer<Base>;
		using Traits = std::pointer_traits<MyPointer>;
		using Index = impl::VariantIndex;
		static constexpr const Index npos = Index(-1);

		static constexpr const size_t Count = sizeof...(Derived);

		template <class T>
		static consteval bool TypeIsCompatible()
		{
			return impl::TypeInVariant<T, Derived...>;
		}

	protected:

		template <template <class> class OtherPointer, std::polymorphic_type OtherBase, std::derived_from<OtherBase>... OtherDerived>
		friend class PointerDynamicVariant;

		using This = PointerDynamicVariant;

		template <class T>
		using TypeIsInMyVariant = impl::TypeIsInVariant<T, Derived...>;



		MyPointer _ptr = {};

		constexpr PointerDynamicVariant(MyPointer const& p):
			_ptr(p)
		{}

		constexpr PointerDynamicVariant(MyPointer && p) :
			_ptr(std::move(p))
		{}



	public:

		constexpr PointerDynamicVariant() noexcept = default;
		constexpr PointerDynamicVariant(PointerDynamicVariant const&) = default;
		constexpr PointerDynamicVariant(PointerDynamicVariant &&) noexcept = default;

		constexpr PointerDynamicVariant(nullptr_t) noexcept {};

		template <impl::TypeInVariant<Derived...>... OtherDerived>
		constexpr PointerDynamicVariant(PointerDynamicVariant<_Pointer, Base, OtherDerived...> const& other):
			_ptr(other._ptr)
		{}

		template <impl::TypeInVariant<Derived...>... OtherDerived>
		constexpr PointerDynamicVariant(PointerDynamicVariant<_Pointer, Base, OtherDerived...> && other) noexcept :
			_ptr(std::move(other._ptr))
		{}

		template <impl::TypeInVariant<Derived...> T>
		constexpr PointerDynamicVariant(_Pointer<T> const& ptr):
			_ptr(ptr)
		{}

		template <impl::TypeInVariant<Derived...> T>
		constexpr PointerDynamicVariant(_Pointer<T> && ptr) noexcept :
			_ptr(std::move(ptr))
		{}


		constexpr PointerDynamicVariant& operator=(PointerDynamicVariant const&) = default;
		constexpr PointerDynamicVariant& operator=(PointerDynamicVariant&&) noexcept = default;

		constexpr PointerDynamicVariant& operator=(nullptr_t) noexcept
		{
			_ptr = nullptr;
			return *this;
		};

		template <impl::TypeInVariant<Derived...>... OtherDerived>
		constexpr PointerDynamicVariant& operator=(PointerDynamicVariant<_Pointer, Base, OtherDerived...> const& other)
		{
			_ptr = other._ptr;
			return *this;
		}

		template <impl::TypeInVariant<Derived...>... OtherDerived>
		constexpr PointerDynamicVariant& operator=(PointerDynamicVariant<_Pointer, Base, OtherDerived...>&& other) noexcept
		{
			_ptr = std::move(other._ptr);
			return *this;
		}

		template <impl::TypeInVariant<Derived...> T>
		constexpr PointerDynamicVariant& operator=(_Pointer<T> const& ptr)
		{
			_ptr = ptr;
			return *this;
		}

		template <impl::TypeInVariant<Derived...> T>
		constexpr PointerDynamicVariant& operator=(_Pointer<T>&& ptr) noexcept
		{
			_ptr = std::move(ptr);
			return *this;
		}


		constexpr void reset() noexcept
		{
			_ptr = nullptr;
		}

		Base* getRaw() const noexcept
		{
			return std::to_address(_ptr);
		}

		PointerDynamicVariant<RawPointer, _Base, Derived...> getRawVariant() const noexcept
		{
			return PointerDynamicVariant<RawPointer, _Base, Derived...>(getRaw());
		}

		MyPointer const& get() const noexcept
		{
			return _ptr;
		}

		constexpr bool checkInvariant() const noexcept
		{
			bool res = true;
			if (_ptr)
			{
				Base* const raw = getRaw();
				// _ptr must be of the type of at least one Derived
				res &= ((dynamic_cast<Derived*>(raw) != nullptr) || ...);
			}
			return res;
		}

		constexpr operator bool() const noexcept
		{
			return static_cast<bool>(_ptr);
		}

		// Zero based index of the actual Derived type
		constexpr Index indexNonEmpty() const noexcept
		{
			assert(operator bool());
			return impl::TestIndex<Derived...>(getRaw());
		}

		constexpr Index index() const noexcept
		{
			if (_ptr)
			{
				return indexNonEmpty();
			}
			else
			{
				return npos;
			}
		}

		template <std::derived_from<Base> Derived>
		constexpr bool is() const
		{
			return dynamic_cast<Derived*>(getRaw());
		}

		// using static_cast
		template <std::derived_from<Base> Derived>
		constexpr Derived* as() const
		{
			assert(getRaw() == nullptr || is<Derived>());
			return static_cast<Derived*>(getRaw());
		}

		template <template <class> class OtherPointer, std::polymorphic_type OtherBase, std::derived_from<OtherBase>... OtherDerived>
		constexpr auto operator<=>(PointerDynamicVariant<OtherPointer, OtherBase, OtherDerived...> const& rhs) const noexcept
		{
			return _ptr <=> rhs._ptr;
		}

		template <class T>
		constexpr auto operator <=>(const T* rhs) const
		{
			return _ptr <=> rhs;
		}

		constexpr size_t hash() const noexcept
		{
			return std::hash<MyPointer>{}(_ptr);
		}

		constexpr decltype(auto) operator*() const noexcept
		{
			return *_ptr;
		}

		constexpr decltype(auto) operator->() const noexcept
		{
			if constexpr (std::is_pointer<MyPointer>::value)
			{
				return _ptr;
			}
			else
			{
				return _ptr.operator->();
			}
		}

		template <impl::TypeInVariant<Derived...> T>
		static PointerDynamicVariant& ReinterpretAsVariant(_Pointer<T>& p) noexcept
		{
			assert(that::CheckReinterpretCastSameAsStaticCast<T>(std::to_address(p)));
			return reinterpret_cast<PointerDynamicVariant&>(p);
		}

		template <impl::TypeInVariant<Derived...> T>
		static const PointerDynamicVariant& ReinterpretAsVariant(_Pointer<T> const& p) noexcept
		{
			assert(that::CheckReinterpretCastSameAsStaticCast<T>(std::to_address(p)));
			return reinterpret_cast<PointerDynamicVariant const&>(p);
		}

		// Assumes this != nullptr
		template <std::concepts::visitor<Derived...> Visitor>
		constexpr typename std::visitor_result<Visitor, Derived...>::type visit(Visitor&& vis) const
		{
			assert(operator bool());
			return impl::TestVisitor<typename std::visitor_result<Visitor, Derived...>::type, Derived...>(std::forward<Visitor>(vis), getRaw());
		}

		// this may be nullptr, handled by the visitor
		template <std::concepts::visitor<nullptr_t, Derived...> Visitor>
		constexpr typename std::visitor_result<Visitor, nullptr_t, Derived...>::type visitMaybeEmpty(Visitor&& vis) const
		{
			if (_ptr)
			{
				return visit(std::forward<Visitor>(vis));
			}
			else
			{
				return vis(nullptr);
			}
		}
	};

	template <class Base, std::derived_from<Base>... Derived>
	using RawPointerDynamicVariant = PointerDynamicVariant<RawPointer, Base, Derived...>;

	template <class Base, std::derived_from<Base>... Derived>
	using UniquePointerDynamicVariant = PointerDynamicVariant<std::unique_ptr, Base, Derived...>;

	template <class Base, std::derived_from<Base>... Derived>
	using SharedPointerDynamicVariant = PointerDynamicVariant<std::shared_ptr, Base, Derived...>;

	template <class Base, std::derived_from<Base>... Derived>
	using WeakPointerDynamicVariant = PointerDynamicVariant<std::weak_ptr, Base, Derived...>;
}