#pragma once

#include <vector>

namespace that
{
	template <class T, class Alloc = std::allocator<T>>
	class ExtensibleStorage
	{
	public:

		using StorageType = std::vector<T, Alloc>;
		using IndexType = typename StorageType::size_type;
	
	protected:

		StorageType _storage;

	public:

		constexpr ExtensibleStorage() = default;

		constexpr ~ExtensibleStorage() = default;

		constexpr ExtensibleStorage(StorageType const& other) :
			_storage(other)
		{}

		constexpr ExtensibleStorage(StorageType && other) noexcept :
			_storage(std::move(other))
		{}

		constexpr ExtensibleStorage(ExtensibleStorage const& other) = default;

		constexpr ExtensibleStorage(ExtensibleStorage&& other) noexcept = default;

		constexpr ExtensibleStorage& operator=(ExtensibleStorage const& other) = default;

		constexpr ExtensibleStorage& operator=(ExtensibleStorage&& other) noexcept = default;

		constexpr ExtensibleStorage& operator=(StorageType const& other)
		{
			_storage = other;
			return *this;
		}

		constexpr ExtensibleStorage& operator=(StorageType && other) noexcept
		{
			_storage = std::move(other);
			return *this;
		}

		void swap(ExtensibleStorage& other)
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

		template <std::convertible_to<T> Q = T>
		IndexType pushBack(const Q * t, IndexType count = 1)
		{
			const IndexType res = _storage.size();
			growIFN(count);
			if (t)
			{
				std::copy_n(t, count, data() + res);
			}
			return res;
		}

		template <std::convertible_to<T> Q>
		IndexType pushBackMove(Q* t, IndexType count = 1)
		{
			const IndexType res = _storage.size();
			growIFN(count);
			assert(!!t);
			for (IndexType i = 0; i < count; ++i)
			{
				data()[res + i] = std::move(t[i]);
			}
			return res;
		}

		constexpr const T* data() const
		{
			return _storage.data();
		}

		constexpr T* data()
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

		constexpr const std::vector<T, Alloc>& getStorage() const
		{
			return _storage;
		}

		constexpr std::vector<T, Alloc>& getStorage()
		{
			return _storage;
		}
	};

	template <class T, class Alloc = std::allocator<T>>
	using ExS = ExtensibleStorage<T, Alloc>;
}

namespace std
{
	template<class T, class Alloc = std::allocator<T>>
	void swap(that::ExtensibleStorage<T, Alloc>& l, that::ExtensibleStorage<T, Alloc>& r)
	{
		l.swap(r);
	}
}