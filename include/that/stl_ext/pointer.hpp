#pragma once

#include <memory>
#include <concepts>
#include <cassert>

namespace that
{
	// This should be doable consteval, but I haven't found a way to do it.
	template <class Derived, class Base>
		requires std::derived_from<Derived, Base>
	static inline bool CheckReinterpretCastSameAsStaticCast(const Base* b)
	{
		const Derived* s = static_cast<const Derived*>(b);
		const Derived* r = reinterpret_cast<const Derived*>(b);
		return s == r;
	}
}

namespace std
{
	// Given a ptr to Base which is already known to point to a Derived,
	// Cast the shared_ptr to Derived without copying it.
	template <class Derived, class Base, template <class> class _Pointer>
		requires std::derived_from<Derived, Base>
		// && std::is_pointer_interconvertible_base_of<Base, Derived>::value // TODO find a way to check a compile time the reinterpret cast compatibility of Base and Derived pointers
	_Pointer<Derived> const& reinterpret_pointer_downcast(_Pointer<Base> const& ptr)
	{
		assert(dynamic_cast<const Derived*>(std::to_address(ptr)) == reinterpret_cast<const Derived*>(std::to_address(ptr))); // Check that ptr is indeed Derived
		assert(that::CheckReinterpretCastSameAsStaticCast<Derived>(std::to_address(ptr))); // This should be doable at compile time, maybe redundant with the above check
		return reinterpret_cast<const _Pointer<Derived>&>(ptr);
	}

	// Given a ptr to Base which is already known to point to a Derived,
	// Cast the shared_ptr to Derived without copying it.
	template <class Derived, class Base, template <class> class _Pointer>
		requires std::derived_from<Derived, Base>
	// && std::is_pointer_interconvertible_base_of<Base, Derived>::value // TODO find a way to check a compile time the reinterpret cast compatibility of Base and Derived pointers
	_Pointer<Derived> & reinterpret_pointer_downcast(_Pointer<Base> & ptr)
	{
		assert(dynamic_cast<const Derived*>(std::to_address(ptr)) == reinterpret_cast<const Derived*>(std::to_address(ptr))); // Check that ptr is indeed Derived
		assert(that::CheckReinterpretCastSameAsStaticCast<Derived>(std::to_address(ptr))); // This should be doable at compile time, maybe redundant with the above check
		return reinterpret_cast<_Pointer<Derived>&>(ptr);
	}
}