
#include <utils/ExtensibleDataStorage.hpp>


namespace that
{
	void ExtensibleDataStorage::reserve(IndexType capacity)
	{
		_storage.reserve(capacity);
	}

	void ExtensibleDataStorage::reserveAdditionalIFN(IndexType extra_capacity)
	{
		const IndexType needed_capacity = size() + extra_capacity;
		if (needed_capacity > capacity())
		{
			reserve(needed_capacity);
		}
	}

	void ExtensibleDataStorage::resize(IndexType size)
	{
		_storage.resize(size);
	}
	
	void ExtensibleDataStorage::growIFN(IndexType needed)
	{
		_storage.resize(needed + _storage.size());
	}

	void ExtensibleDataStorage::shrink()
	{
		_storage.shrink_to_fit();
	}

	void ExtensibleDataStorage::clear()
	{
		_storage.clear();
	}

	ExtensibleDataStorage::IndexType ExtensibleDataStorage::pushBack(const void* data, IndexType size)
	{
		const IndexType res = static_cast<IndexType>(_storage.size());
		growIFN(size);
		std::memcpy(this->data() + res, data, size);
		return res;
	}

	void ExtensibleDataStorage::swap(ExtensibleDataStorage& other) noexcept
	{
		_storage.swap(other._storage);
	}
}