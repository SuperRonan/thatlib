#pragma once

#include <that/core/Strings.hpp>

#ifndef THAT_STRING_CONTAINER_USE_THREAD_LOCAL_TMP
#define THAT_STRING_CONTAINER_USE_THREAD_LOCAL_TMP 1
#endif

namespace that
{
	template <class char_t>
	class StringContainerBase
	{
	public:

		using Char = char_t;
		using String = std::basic_string<Char>;
		using StringView = std::basic_string_view<Char>;
	
	protected:

		template <concepts::GenericString Str>
		static String 
#if THAT_STRING_CONTAINER_USE_THREAD_LOCAL_TMP
		const& 
#endif
		Convert(Str const& src)
		{
#if THAT_STRING_CONTAINER_USE_THREAD_LOCAL_TMP
			static thread_local 
#endif
			String tmp;
			ConvertString<Str, Char>(src, tmp);
			return tmp;
		}

	public:

		constexpr StringContainerBase() noexcept = default;

		constexpr ~StringContainerBase() noexcept = default;
	};

	template <class char_t, template <class C> class Container>
	class StringContainerImpl : public StringContainerBase<char_t>
	{
	public:

		using ParentType = StringContainerBase<char_t>;

		using String = ParentType::String;

		using Container_t = Container<String>;
		using Iterator = typename Container<String>::iterator;
		using ConstIterator = typename Container<String>::const_iterator;

	protected:

		Container_t _container;

		template <concepts::GenericString Str>
		static String const& Convert(Str const& src)
		{
			return ParentType::template Convert<Str>(src);
		}

	public:

		StringContainerImpl() noexcept = default;

		~StringContainerImpl() = default;

		Container_t& getUnderlyingContainer() noexcept
		{
			return _container;
		}

		Container_t const& getUnderlyingContainer() const noexcept
		{
			return _container;
		}

		size_t size() const noexcept
		{
			return _container.size();
		}

		size_t max_size() const noexcept
		{
			return _container.max_size();
		}

		void clear() noexcept
		{
			_container.clear();
		}

		bool empty() const noexcept
		{
			return _container.empty();
		}

		bool contains(String const& str) const noexcept
		{
			return _container.contains(str);
		}

		template <concepts::GenericString Str>
		bool contains(Str const& str) const noexcept
		{
			return contains(Convert(str));
		}

		size_t erase(String const& str) noexcept
		{
			return _container.erase(str);
		}

		template <concepts::GenericString Str>
		size_t erase(Str const& str) noexcept
		{
			return erase(Convert(str));
		}
	};
}