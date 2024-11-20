#pragma once

#include <that/utils/StringContainer.hpp>
#include <unordered_set>

namespace that
{
	template <class char_t>
	class StringSet : public StringContainerImpl<char_t, std::unordered_set>
	{
	public:

	protected:

		using ParentType = StringContainerImpl<char_t, std::unordered_set>;
		using String = ParentType::String;

	public:

		StringSet() = default;
		~StringSet() = default;

		decltype(auto) insert(String const& str)
		{
			return ParentType::getUnderlyingContainer().insert(str);
		}

		template <concepts::GenericString Str>
		decltype(auto) insert(Str const& str)
		{
			return insert(ParentType::Convert(str));
		}

		decltype(auto) insert(String&& str)
		{
			return ParentType::getUnderlyingContainer().insert(std::move(str));
		}
	};

} // namespace that
