#pragma once

#include <that/utils/StringContainer.hpp>

#include <unordered_map>

namespace that
{
	namespace impl
	{
		template <class Value_t>
		struct GetStringMapContainer
		{
			template <class Key>
			using Type = std::unordered_map<Key, Value_t>;
		};
	}
	template <class char_t, class Value_t = std::basic_string<char_t>>
	class StringMap : public StringContainerImpl<char_t, typename impl::GetStringMapContainer<Value_t>::Type>
	{
	public:
		
		using ParentType = StringContainerImpl<char_t, typename impl::GetStringMapContainer<Value_t>::Type>;

		using String = ParentType::String;

		using Value = Value_t;

	protected:

	public:

		StringMap() = default;
		~StringMap() = default;

		Value& at(const String& key)
		{
			return ParentType::getUnderlyingContainer().at(key);
		}

		template <concepts::GenericString Str>
		Value& at(Str const& key)
		{
			return at(ParentType::Convert(key));
		}

		Value const& at(const String& key) const
		{
			return ParentType::getUnderlyingContainer().at(key);
		}

		template <concepts::GenericString Str>
		Value const& at(const Str& key) const
		{
			return at(ParentType::Convert(key));
		}

		Value& operator[](String const& key)
		{
			return ParentType::getUnderlyingContainer()[key];
		}

		template <concepts::GenericString Str>
		Value& operator[](Str const& key)
		{
			return operator[](ParentType::Convert(key));
		}

		Value& operator[](String&& key)
		{
			return ParentType::getUnderlyingContainer()[std::move(key)];
		}
	};

} // namespace that
