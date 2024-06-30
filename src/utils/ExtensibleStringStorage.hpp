#pragma once

#include <core/Strings.hpp>
#include <core/Range.hpp>
#include <vector>
#include <cassert>
#include <cstdarg>

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


	protected:

		std::vector<char_t> _storage = {};

	public:

		constexpr ExtensibleBasicStringStorage() = default;

		constexpr ~ExtensibleBasicStringStorage() = default;

		constexpr ExtensibleBasicStringStorage(ExtensibleBasicStringStorage const& other) = default;

		constexpr ExtensibleBasicStringStorage(ExtensibleBasicStringStorage && other) noexcept = default;

		ExtensibleBasicStringStorage& operator=(ExtensibleBasicStringStorage const& other) = default;

		ExtensibleBasicStringStorage& operator=(ExtensibleBasicStringStorage && other) noexcept = default;

		void swap(ExtensibleBasicStringStorage& other)
		{
			_storage.swap(other._storage);
		}

		void reserve(IndexType capacity)
		{
			_storage.reserve(capacity);
		}

		void reserveAdditionalIFN(IndexType extra_capacity)
		{
			const IndexType needed_capacity = size() + extra_capacity;
			if (needed_capacity > capacity())
			{
				reserve(needed_capacity);
			}
		}

		void resize(IndexType size)
		{
			_storage.resize(size);
		}

		void growIFN(IndexType needed)
		{
			_storage.resize(needed + _storage.size());
		}

		void shrink()
		{
			_storage.shrink_to_fit();
		}

		void clear()
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

		IndexType pushBack(std::string_view const& sv, bool terminate_null = true)
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

		void pushNull()
		{
			growIFN(1);
			_storage.back() = char_t(0);
		}
		
		template <std::integral Index>
		StringViewType get(Range<Index> const& r)const
		{
			assert(r.end() < size());
			const StringViewType res(data() + r.begin, r.len);
			return res;
		}

		Range<IndexType> print(bool terminate_null, const char_t* format, ...)
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
	};

	using ExtensibleStringStorage = ExtensibleBasicStringStorage<char>;

	template <class char_t>
	using ExBSS = ExtensibleBasicStringStorage<char_t>;

	using ExSS = ExBSS<char>;
}

namespace std
{
	template <class char_t>
	void swap(that::ExtensibleBasicStringStorage<char_t>& l, that::ExtensibleBasicStringStorage<char_t>& r)
	{
		l.swap(r);
	}
}