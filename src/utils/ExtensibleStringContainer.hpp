#pragma once

#include "ExtensibleStringStorage.hpp"
#include <core/Range.hpp>

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

		using Range = ::that::Range<IndexType>;
		//using Ranges = std::vector<Range>;

	protected:

		StorageType _storage;
		std::vector<IndexType> _indices;

	public:

		constexpr ExtensibleBasicStringContainer() = default;
		
		constexpr ~ExtensibleBasicStringContainer() = default;

		constexpr ExtensibleBasicStringContainer(ExtensibleBasicStringContainer const&) = default;
		constexpr ExtensibleBasicStringContainer(ExtensibleBasicStringContainer &&) noexcept = default;

		ExtensibleBasicStringContainer& operator=(ExtensibleBasicStringContainer const&) = default;
		ExtensibleBasicStringContainer& operator=(ExtensibleBasicStringContainer &&) noexcept= default;

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

		std::string_view operator[](size_t i) const
		{
			assert(i < size());
			return _storage.get(getRange(i));
		}

		std::string_view at(size_t i) const
		{
			std::string_view res;
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

			operator std::string_view() const
			{
				return _that->_storage.get(_that->getBackRange());
			}

			BackStringReference& operator=(std::string_view const& sv)
			{
				clear();
				_that->_storage.pushBack(sv, true);
				return *this;
			}

			BackStringReference& operator+=(std::string_view const& sv)
			{
				
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
		};

		BackStringReference back()
		{
			assert(!empty());
			return BackStringReference(this);
		}

		std::string_view back() const
		{
			assert(!empty());
			return _storage.get(getBackRange());
		}

		void pushBack()
		{
			const size_t index = _storage.pushNull();
			_indices.push_back(index);
		}

		void pushBack(std::string_view const& sv)
		{
			const size_t index = _storage.pushBack(sv, true);
			_indices.push_back(index);
		}

		void push_back(std::string_view const& sv)
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

		ExtensibleBasicStringContainer& operator+=(std::string_view const& sv)
		{
			pushBack(sv);
			return *this;
		}

		ExtensibleBasicStringContainer& operator+=(ExtensibleBasicStringContainer const& other)
		{
			const std::string_view sv(other._storage.data(), other._storage.size());
			const IndexType offset = _storage.pushBack(sv, false);
			const size_t prev_size = _indices.size();
			_indices.resize(prev_size + other._indices.size());
			for (size_t i = 0; i < other._indices.size(); ++i)
			{
				_indices[prev_size + i] = other._indices[i] + offset;
			}
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