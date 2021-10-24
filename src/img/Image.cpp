#include "Image.h"

namespace img
{
	constexpr FormatlessImage::FormatlessImage(size_t w, size_t h, size_t elem_size):
		_w(w),
		_h(h),
		_size(w * h),
		_buffer(_size * elem_size)
	{}

	constexpr size_t FormatlessImage::width()const
	{
		return _w;
	}

	constexpr size_t FormatlessImage::height()const
	{
		return _h;
	}

	constexpr size_t FormatlessImage::size()const
	{
		return _size;
	}

	constexpr size_t FormatlessImage::byteSize()const
	{
		return _buffer.size();
	}

	constexpr const FormatlessImage::byte* FormatlessImage::rawData()const
	{
		return _buffer.data();
	}

	constexpr FormatlessImage::byte* FormatlessImage::rawData()
	{
		return _buffer.data();
	}

	constexpr decltype(auto) FormatlessImage::rawBegin()
	{
		return _buffer.begin();
	}

	constexpr decltype(auto) FormatlessImage::rawBegin()const
	{
		return _buffer.begin();
	}

	constexpr decltype(auto) FormatlessImage::rawCBegin()const
	{
		return _buffer.cbegin();
	}

	constexpr decltype(auto) FormatlessImage::rawEnd()
	{
		return _buffer.end();
	}

	constexpr decltype(auto) FormatlessImage::rawEnd()const
	{
		return _buffer.end();
	}

	constexpr decltype(auto) FormatlessImage::rawCEnd()const
	{
		return _buffer.cend();
	}

	constexpr bool FormatlessImage::empty()const
	{
		return _size == 0;
	}

	constexpr void FormatlessImage::resize(size_t w, size_t h, size_t elem_size)
	{
		_w = w;
		_h = h;
		_size = _w * _h;
		_buffer.resize(_size * elem_size);
	}
}