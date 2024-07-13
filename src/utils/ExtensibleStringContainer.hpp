#pragma once

#include "ExtensibleStringStorage.hpp"
#include <core/Range.hpp>
#include <core/Strings.hpp>

namespace that
{
	// First implementation:
	// All strings are contiguous in storage and null terminated
	// Only the last one can be edited
	template <class char_t, std::integral IndexType = size_t>
	class ExtensibleBasicStringContainer
	{
	public:

		using CharType = char_t;
		using StorageType = ExtensibleBasicStringStorage<char_t>;
		using StringType = std::basic_string<char_t>;
		using StringViewType = std::basic_string_view<char_t>;

		template <class ... Args>
		using FormatString = std::basic_format_string<char_t, std::type_identity_t<Args>...>;

		using Range = ::that::Range<IndexType>;
		//using Ranges = std::vector<Range>;

	protected:

		StorageType _storage;
		std::vector<IndexType> _indices;

		template <concepts::BasicStringLike<char_t> Str>
		void pushFromInitList(std::initializer_list<Str> const& list)
		{
			_indices.reserve(_indices.size() + list.size());
			if (list.size() > 0)
			{
				const Str& str = *list.begin();
				_storage.reserve(_storage.size() + (GetBasicStringSize<char_t>(str) + 1) * list.size());
			}
			for (const auto& s : list)
			{
				pushBack(s);
			}
		}

	public:

		constexpr ExtensibleBasicStringContainer() = default;
		
		constexpr ~ExtensibleBasicStringContainer() = default;

		constexpr ExtensibleBasicStringContainer(ExtensibleBasicStringContainer const&) = default;
		constexpr ExtensibleBasicStringContainer(ExtensibleBasicStringContainer &&) noexcept = default;

		
		template <concepts::BasicStringLike<char_t> Str>
		ExtensibleBasicStringContainer(std::initializer_list<Str> const& list)
		{
			pushFromInitList(list);
		}

		ExtensibleBasicStringContainer& operator=(ExtensibleBasicStringContainer const&) = default;
		ExtensibleBasicStringContainer& operator=(ExtensibleBasicStringContainer &&) noexcept= default;

		template <concepts::BasicStringLike<char_t> Str>
		ExtensibleBasicStringContainer& operator=(std::initializer_list<Str> const& list)
		{
			_indices.clear();
			_storage.clear();
			pushFromInitList(list);
			return *this;
		}

		void swap(ExtensibleBasicStringContainer& other) noexcept
		{
			_storage.swap(other._storage);
			_indices.swap(other._indices);
		}

		StorageType& storage()
		{
			return _storage;
		}

		StorageType const& storage() const
		{
			return _storage;
		}

		Range getRange(size_t i) const
		{
			assert(i < _indices.size());
			const IndexType next_begin = ((i + 1) == _indices.size()) ? _storage.size() : _indices[i + 1];
			const Range res{ .begin = _indices[i], .len = (next_begin - _indices[i] - 1)};
			return res;
		}

		Range getBackRange() const
		{
			assert(!empty());
			return Range{
				.begin = _indices.back(),
				.len = (_storage.size() - _indices.back() - 1),
			};
		}

		constexpr size_t size() const
		{
			return _indices.size();
		}

		constexpr bool empty() const
		{
			return _indices.empty();
		}

		void clear()
		{
			_storage.clear();
			_indices.clear();
		}

		StringViewType operator[](size_t i) const
		{
			assert(i < size());
			return _storage.get(getRange(i));
		}

		StringViewType at(size_t i) const
		{
			StringViewType res;
			if (i < size())
			{
				res = operator[](i);
			}
			return res;
		}

		class BackStringReference
		{
		protected:
			
			ExtensibleBasicStringContainer * _that;

		public:

			BackStringReference(ExtensibleBasicStringContainer * exc):
				_that(exc)
			{}

			IndexType index() const
			{
				return _that->_indices.back();
			}

			void clear()
			{
				_that->_storage.resize(index());
			}

			operator StringViewType() const
			{
				return _that->_storage.get(_that->getBackRange());
			}

			BackStringReference& operator=(StringViewType const& sv)
			{
				clear();
				_that->_storage.pushBack(sv, true);
				return *this;
			}

			BackStringReference& operator+=(StringViewType const& sv)
			{
				_that->_storage.pop(1);
				_that->_storage.pushBack(sv, true);
				return *this;
			}

			void printf(bool append, const char_t* format, ...)
			{
				std::va_list args;
				va_start(args, format);
				if (append)
				{
					// pop null terminator
					_that->_storage.pop(1);
				}
				else
				{
					clear();
				}
				_that->_storage.printf(true, format, args);
				va_end(args);
			}

			template <class ...Args>
			void format(bool append, FormatString<Args...> fmt, Args&& ... args)
			{
				if (append)
				{
					_that->_storage.pop(1);
				}
				else
				{
					clear();
				}
				_that->_storage.pushBackFormatted(true, fmt, std::forward<Args>(args)...);
			}
		};

		BackStringReference back()
		{
			assert(!empty());
			return BackStringReference(this);
		}

		const StringViewType back() const
		{
			assert(!empty());
			return _storage.get(getBackRange());
		}

		void pushBack()
		{
			const size_t index = _storage.pushNull();
			_indices.push_back(index);
		}

		void pushBack(StringViewType const& sv)
		{
			const size_t index = _storage.pushBack(sv, true);
			_indices.push_back(index);
		}

		void printfBack(const char_t* format, ...)
		{
			std::va_list args;
			va_start(args, format);
			pushBack();
			back().printf(false, format, args);
			va_end(args);
		}

		template <class ... Args>
		void pushBackFormatted(FormatString<Args...> fmt, Args&& ... args)
		{
			pushBack();
			back().format(false, fmt, std::forward<Args>(args)...);
		}

		void push_back(StringViewType const& sv)
		{
			pushBack(sv);
		}

		void popBack()
		{
			assert(!empty());
			const IndexType prev = _indices.back();
			_indices.pop_back();
			_storage.resize(prev);
		}

		void pop_back()
		{
			popBack();
		}

		ExtensibleBasicStringContainer& operator+=(StringViewType const& sv)
		{
			pushBack(sv);
			return *this;
		}

		ExtensibleBasicStringContainer& operator+=(ExtensibleBasicStringContainer const& other)
		{
			const StringViewType sv(other._storage.data(), other._storage.size());
			const IndexType offset = _storage.pushBack(sv, false);
			const size_t prev_size = _indices.size();
			_indices.resize(prev_size + other._indices.size());
			for (size_t i = 0; i < other._indices.size(); ++i)
			{
				_indices[prev_size + i] = other._indices[i] + offset;
			}
			return *this;
		}

		template <concepts::BasicStringLike<char_t> Str>
		ExtensibleBasicStringContainer& operator+=(std::initializer_list<Str> const& list)
		{
			pushFromInitList(list);
			return *this;
		}
	};


	using ExtensibleStringContainer = ExtensibleBasicStringContainer<char>;
	using ExtensibleStringContainer32 = ExtensibleBasicStringContainer<char, uint32_t>;
	using ExtensibleWideStringContainer = ExtensibleBasicStringContainer<wchar_t>;
	using ExtensibleWideStringContainer32 = ExtensibleBasicStringContainer<wchar_t, uint32_t>;

	template <class char_t, std::integral IndexType = size_t>
	using ExBSC = ExtensibleBasicStringContainer<char_t, IndexType>;

	using ExSC = ExBSC<char>;
	using ExSC32 = ExBSC<char, uint32_t>;
	using ExWSC = ExBSC<wchar_t>;
	using ExWSC32 = ExBSC<wchar_t, uint32_t>;

}

namespace std
{
	template <class char_t>
	void swap(that::ExtensibleBasicStringContainer<char_t>& l, that::ExtensibleBasicStringContainer<char_t>& r)
	{
		l.swap(r);
	}
}