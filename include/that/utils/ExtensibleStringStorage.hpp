#pragma once

#include <that/core/Core.hpp>
#include <that/core/Strings.hpp>
#include <that/core/Range.hpp>
#include <vector>
#include <cassert>
#include <cstdarg>
#include <format>

namespace that
{
	// Stores null terminated strings in one contiguous memory chunk
	template <class char_t>
	class ExtensibleBasicStringStorage
	{
	public:

		using IndexType = size_t;
		using CharType = char_t;
		using StringType = std::basic_string<char_t>;
		using StringViewType = std::basic_string_view<char_t>;

		template <class ... Args>
		using FormatString = std::basic_format_string<char_t, std::type_identity_t<Args>...>;

	protected:

		std::vector<char_t> _storage = {};

	public:

		constexpr ExtensibleBasicStringStorage() = default;

		constexpr ~ExtensibleBasicStringStorage() = default;

		constexpr ExtensibleBasicStringStorage(ExtensibleBasicStringStorage const& other) = default;

		constexpr ExtensibleBasicStringStorage(ExtensibleBasicStringStorage && other) noexcept = default;

		constexpr ExtensibleBasicStringStorage& operator=(ExtensibleBasicStringStorage const& other) = default;

		constexpr ExtensibleBasicStringStorage & operator=(ExtensibleBasicStringStorage && other) noexcept = default;

		constexpr void swap(ExtensibleBasicStringStorage& other) noexcept
		{
			_storage.swap(other._storage);
		}

		constexpr void reserve(IndexType capacity)
		{
			_storage.reserve(capacity);
		}

		constexpr void reserveAdditionalIFN(IndexType extra_capacity)
		{
			const IndexType needed_capacity = size() + extra_capacity;
			if (needed_capacity > capacity())
			{
				reserve(needed_capacity);
			}
		}

		constexpr void resize(IndexType size)
		{
			_storage.resize(size);
		}

		constexpr void growIFN(IndexType needed)
		{
			_storage.resize(needed + _storage.size());
		}

		constexpr void pop(IndexType to_remove = 1)
		{
			assert(_storage.size() >= to_remove);
			_storage.resize(_storage.size() - to_remove);
		}

		constexpr void shrink()
		{
			_storage.shrink_to_fit();
		}

		constexpr void clear()
		{
			_storage.clear();
		}

		constexpr const char_t* data() const
		{
			return _storage.data();
		}

		constexpr char_t* data()
		{
			return _storage.data();
		}

		constexpr IndexType size()const
		{
			return static_cast<IndexType>(_storage.size());
		}

		constexpr IndexType capacity()const
		{
			return static_cast<IndexType>(_storage.capacity());
		}

		IndexType pushBack(StringViewType const& sv, bool terminate_null = true)
		{
			const IndexType res = size();
			growIFN(sv.size() + (terminate_null ? 1 : 0));
			std::memcpy(_storage.data() + res, sv.data(), sv.size());
			if (terminate_null)
			{
				_storage.back() = char_t(0);
			}
			return res;
		}

		IndexType pushNull()
		{
			const IndexType res = size();
			growIFN(1);
			_storage.back() = char_t(0);
			return res;
		}
		
		template <std::integral Index>
		StringViewType get(Range<Index> const& r)const
		{
			assert(r.end() < size());
			const StringViewType res(data() + r.begin, r.len);
			return res;
		}

		Range<IndexType> printf(bool terminate_null, const char_t* format, ...)
		{
			static_assert(std::is_same<char_t, char>::value);
			Range<IndexType> res;
			res.begin = size();
			// va_args may be altered by the print function
			std::va_list args1;
			std::va_list args2;
			va_start(args1, format);
			va_copy(args2, args1);
			// Does not include the null terminator
			res.len = std::vsnprintf(nullptr, 0, format, args1);
			va_end(args1);
			growIFN(res.len + 1);
			std::vsnprintf(data() + res.begin, res.len + 1, format, args2);
			va_end(args2);
			if (terminate_null)
			{
				_storage.back() = char_t(0);
			}
			else if(_storage.size() > 0)
			{
				_storage.resize(_storage.size() - 1);
			}
			return res;
		}
		
		template <class ... Args>
		constexpr Range<IndexType> pushBackFormatted(bool terminate_null, FormatString<Args...> fmt, Args && ... args)
		{
			const size_t needed = std::formatted_size(fmt, std::forward<Args>(args)...);
			Range<IndexType> res;
			res.begin = size();
			res.len = needed;
			growIFN(needed + (terminate_null ? 1 : 0));
			char_t * const begin = _storage.data() + res.begin;
			char_t * const end = std::format_to(begin, fmt, std::forward<Args>(args)...);
			const size_t n = (end - begin);
			assert(n <= needed);
			if (n != needed)
			{
				THAT_BREAKPOINT_HANDLE;
			}
			if (terminate_null)
			{
				_storage.back() = char_t(0);
			}
			return res;
		}
	};

	using ExtensibleStringStorage = ExtensibleBasicStringStorage<char>;
	using ExtensibleWideStringStorage = ExtensibleBasicStringStorage<wchar_t>;

	template <class char_t>
	using ExBSS = ExtensibleBasicStringStorage<char_t>;

	using ExSS = ExBSS<char>;
	using ExWSS = ExBSS<wchar_t>;
}

namespace std
{
	template <class char_t>
	constexpr void swap(that::ExtensibleBasicStringStorage<char_t>& l, that::ExtensibleBasicStringStorage<char_t>& r) noexcept
	{
		l.swap(r);
	}
}