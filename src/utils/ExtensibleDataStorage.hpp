#pragma once

#include <vector>
#include <cstdint>

namespace that
{
	class ExtensibleDataStorage
	{
	public:
		
		using IndexType = size_t;
		using AtomType = uint8_t;

	protected:

		std::vector<uint8_t> _storage = {};

	public:

		constexpr ExtensibleDataStorage() = default;

		constexpr ~ExtensibleDataStorage() = default;

		constexpr ExtensibleDataStorage(ExtensibleDataStorage const& other) = default;

		constexpr ExtensibleDataStorage(ExtensibleDataStorage && other) noexcept = default;

		constexpr ExtensibleDataStorage & operator=(ExtensibleDataStorage const& other) = default;

		constexpr ExtensibleDataStorage & operator=(ExtensibleDataStorage && other) noexcept = default;

		void reserve(IndexType capacity);

		void reserveAdditionalIFN(IndexType extra_capacity);

		void resize(IndexType size);

		void growIFN(IndexType needed);
		
		void shrink();

		void clear();
		
		IndexType pushBack(const void* data, IndexType size);

		template <class T>
		IndexType pushBack(T const& t)
		{
			return pushBack(&t, sizeof(T));
		}

		constexpr const AtomType* data() const
		{
			return _storage.data();
		}

		constexpr AtomType* data()
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

		void swap(ExtensibleDataStorage & other) noexcept;
	};


	using ExDS = ExtensibleDataStorage;
}

namespace std
{
	template <>
	inline void swap(that::ExtensibleDataStorage& l, that::ExtensibleDataStorage& r) noexcept
	{
		l.swap(r);
	}
}