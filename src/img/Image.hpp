#pragma once

#include <cassert>
#include <type_traits>
#include <algorithm>
#include <typeinfo>
#include <cassert>
#include <vector>
#include <stdint.h>

#define me (*this)

namespace img
{
	// https://en.wikipedia.org/wiki/Row-_and_column-major_order
	// ---------------------
	// ---------------------
	// ---------------------
	bool constexpr IMAGE_ROW_MAJOR = true;
	// | | | | | | | | | | | | | | | | | | | | |
	// | | | | | | | | | | | | | | | | | | | | |
	// | | | | | | | | | | | | | | | | | | | | |
	bool constexpr IMAGE_COL_MAJOR = false;

	// i -- (column index)
	// j |  (line index)
	template <bool ROW_MAJOR, class uint>
	constexpr uint index(uint i, uint j, uint width, uint height) noexcept
	{
		if constexpr (ROW_MAJOR)	return i + j * width;
		else						return j + i * height;
	}
	
	namespace __img
	{
		template <class T, bool RM>
		struct ImageContent
		{
			const void* img;
		};
	}

	class FormatlessImage
	{
	protected:

		using byte = int8_t;

		// In elements
		size_t _w = 0, _h = 0, _size = 0;
		std::vector<byte> _buffer = {};


	public:

		constexpr FormatlessImage() = default;

		constexpr FormatlessImage(size_t w, size_t h, size_t pixel_size=1):
			_w(w),
			_h(h),
			_size(w * h),
			_buffer(_size * pixel_size)
		{}

		constexpr FormatlessImage(FormatlessImage const&) = default;
		constexpr FormatlessImage(FormatlessImage&&) = default;

		constexpr FormatlessImage& operator=(FormatlessImage const&) = default;
		constexpr FormatlessImage& operator=(FormatlessImage &&) = default;


		constexpr size_t width()const
		{
			return _w;
		}
		constexpr size_t height()const
		{
			return _h;
		}
		constexpr size_t size() const
		{
			return _size;
		}
		constexpr size_t byteSize()const
		{
			return _buffer.size();
		}

		constexpr const byte* rawData()const
		{
			return _buffer.data();
		}
		constexpr byte* rawData()
		{
			return _buffer.data();
		}

		constexpr typename decltype(auto) rawBegin()
		{
			return _buffer.begin();
		}
		constexpr typename decltype(auto) rawBegin()const
		{
			return _buffer.begin();
		}
		constexpr typename decltype(auto) rawCBegin()const
		{
			return _buffer.cbegin();
		}

		constexpr typename decltype(auto) rawEnd()
		{
			return _buffer.end();
		}
		constexpr typename decltype(auto) rawEnd()const
		{
			return _buffer.end();
		}
		constexpr typename decltype(auto) rawCEnd()const
		{
			return _buffer.cend();
		}

		constexpr bool empty()const
		{
			return _size == 0;
		}

		constexpr void resize(size_t w = 0, size_t h = 0, size_t pixel_size = 1)
		{
			_w = w;
			_h = h;
			_size = _w * _h;
			_buffer.resize(_size * pixel_size);
		}

	};

	enum ElementType
	{
		UNORM = 1,
		SNORM = 2,
		UINT = 5,
		SINT = 6,
		sRGB = 7,
		FLOAT = 9,
	};

	struct FormatInfo
	{
		ElementType type;
		uint32_t elem_size;
		uint32_t channels;
	};

	class FormatedImage : public FormatlessImage
	{
	protected:

		FormatInfo _format = {};
		uint32_t _pixel_size = 0;
		bool _row_major = false;

	public:

		FormatedImage() = default;

		FormatedImage(size_t w, size_t h, FormatInfo format, bool row_major = true):
			FormatlessImage(w, h, format.elem_size * format.channels),
			_format(format),
			_pixel_size(_format.elem_size * _format.channels),
			_row_major(row_major)
		{}

		FormatedImage(FormatedImage const& other) :
			FormatlessImage(other),
			_format(other._format),
			_pixel_size(other._pixel_size),
			_row_major(other._row_major)
		{}

		FormatedImage(FormatedImage && other) noexcept :
			FormatlessImage(std::move(other)),
			_format(other._format),
			_pixel_size(other._pixel_size),
			_row_major(other._row_major)
		{}

		FormatedImage& operator=(FormatedImage const& other)
		{
			FormatlessImage::operator=(other);
			_format = other._format;
			_pixel_size = other._pixel_size;
			_row_major = other._row_major;
			return *this;
		}

		FormatedImage& operator=(FormatedImage&& other) noexcept
		{
			FormatlessImage::operator=(std::move(other));
			std::swap(_format, other._format);
			std::swap(_pixel_size, other._pixel_size);
			std::swap(_row_major, other._row_major);
			return *this;
		}

		constexpr FormatInfo format()const
		{
			return _format;
		}

		constexpr size_t pixelSize()const
		{
			return _pixel_size;
		}

		constexpr bool rowMajor()const
		{
			return _row_major;
		}

		size_t pixelLinearIndex(size_t x, size_t y) const
		{
			const size_t res = _row_major ? (y * _w + x) : (x * _h + y);
			return res;
		}

		byte* getPixelPtr(size_t i)
		{
			return rawData() + i * _pixel_size;
		}

		const byte* getPixelPtr(size_t i) const
		{
			return rawData() + i * _pixel_size;
		}

		byte* getPixelPtr(size_t x, size_t y)
		{
			return getPixelPtr(pixelLinearIndex(x, y));
		}

		const byte* getPixelPtr(size_t x, size_t y) const
		{
			return getPixelPtr(pixelLinearIndex(x, y));
		}

		template <class PixelType>
		PixelType& pixel(size_t x, size_t y)
		{
			const size_t i = pixelLinearIndex(x, y);
			PixelType* t_data = static_cast<PixelType*>(rawData());
			return t_data[i];
		}

		template <class PixelType>
		const PixelType& pixel(size_t x, size_t y) const
		{
			const size_t i = pixelLinearIndex(x, y);
			const PixelType* t_data = static_cast<const PixelType*>(rawData());
			return t_data[i];
		}

		template <class ElemType>
		ElemType& pixelComp(size_t x, size_t y, uint32_t c)
		{
			const size_t i = pixelLinearIndex(x, y);
			ElemType * t_data = static_cast<ElementType*>(rawData());
			return t_data[i * _format.channels + c];
		}

		template <class ElemType>
		const ElemType& pixelComp(size_t x, size_t y, uint32_t c)
		{
			const size_t i = pixelLinearIndex(x, y);
			const ElemType* t_data = static_cast<const ElementType*>(rawData());
			return t_data[i * _format.channels + c];
		}

		bool reFormat(FormatInfo const& new_format)
		{
			bool res = false;
			const FormatedImage & old = *this;
			FormatedImage reformated = FormatedImage(old.width(), old.height(), new_format, old.rowMajor());
			
			if (old.format().type == reformated.format().type)
			{
				if (old.format().elem_size == reformated.format().elem_size)
				{
					// For each pixel
					for (size_t i = 0; i < reformated.size(); ++i)
					{
						const byte* old_pixel = old.getPixelPtr(i);
						byte* new_pixel = reformated.getPixelPtr(i);
						// For the last pixel: we read some bytes after the end of the buffer, hope it won't be and issue
						std::memcpy(new_pixel, old_pixel, reformated.pixelSize());
					}

					res = true;
				}
			}
			
			if (res)
			{
				*this = std::move(reformated);
			}

			return res;
		}
	};

	// It is easier to move on major
	template <class T, bool M_ROW_MAJOR = IMAGE_ROW_MAJOR>
	class Image : public FormatlessImage
	{
	protected:

		template <class Q, bool _ROW_MAJOR>
		friend class Image;

		template<class Q, bool _ROW_MAJOR>
		constexpr void copyData(Image<Q, _ROW_MAJOR>const& other)
		{
			if constexpr (M_ROW_MAJOR == _ROW_MAJOR) // likely
			{
				_buffer = other._buffer;
			}
			else
			{
				// TODO more cache efficient (by tile)
				loop2D([this, &other](size_t i, size_t j)
					{
						me(i, j) = other(i, j);
					});
			}
		}

	public:

		static constexpr bool MAJOR()
		{
			return M_ROW_MAJOR;
		}

		using _Type = T;

		constexpr Image(size_t width = 0, size_t height = 0) :
			FormatlessImage(width, height, sizeof(T))
		{}

		constexpr Image(size_t width, size_t height, T const& default_value) :
			FormatlessImage(width, height, sizeof(T))
		{
			std::fill(begin(), end(), default_value);
		}

		constexpr Image(Image const& other):
			FormatlessImage(other)
		{}

		template<class Q, bool _ROW_MAJOR>
		constexpr Image(Image<Q, _ROW_MAJOR> const& other) :
			 FormatlessImage(other.width(), other.height(), other.elemSize())
		{
			copyData(other);
		}

		// TODO template
		constexpr Image(Image && other) noexcept:
			FormatlessImage(std::move(other))
		{}

		template<class Q, bool _ROW_MAJOR>
		constexpr Image& operator=(Image<Q, _ROW_MAJOR> const& other) noexcept
		{
			resize(other.width(), other.height(), other.elemSize());
			copyData(other);
			return me;
		}

		constexpr Image& operator=(Image const& other) noexcept
		{
			FormatlessImage::operator=(std::move(other));
			return me;
		}

		constexpr size_t elemSize()const
		{
			return sizeof(T);
		}

		constexpr T* begin()
		{
			return (T*)rawData();
		}

		constexpr T* end()
		{
			return (T*) rawEnd() + size();
		}

		constexpr const T* begin()const
		{
			return (const T*)rawBegin();
		}

		constexpr const T* end()const
		{
			return (const T*)rawEnd() + size();
		}

		constexpr const T* cbegin()const
		{
			return (T*)rawCBegin();
		}

		constexpr const T* cend()const
		{
			return (T*)rawCEnd() + size();
		}

		// i -- (column index)
		// j |  (line index)
		constexpr size_t index(size_t i, size_t j)const noexcept
		{
			return relativeIndex(i, j);
		}

		// i -- (column index)
		// j |  (line index)
		constexpr size_t relativeIndex(size_t i, size_t j)const noexcept
		{
			return img::index<M_ROW_MAJOR, size_t>(i, j, width(), height());
		}

		constexpr size_t fixedIndex(size_t major_index, size_t minor_index)const noexcept
		{
			return major_index + majorSize() * minor_index;
		}

		constexpr T const& fixedAccess(size_t major_index, size_t minor_index) const
		{
			assert(major_index >= 0);
			assert(minor_index >= 0);
			assert(major_index < majorSize());
			assert(minor_index < minorSize());
			return me[fixedIndex(major_index, minor_index)];
		}

		constexpr size_t majorSize()const
		{
			if constexpr (M_ROW_MAJOR)	return width();
			else						return height();
		}

		constexpr size_t minorSize()const
		{
			if constexpr (M_ROW_MAJOR)	return height();
			else						return width();
		}

		template <class Function>
		__forceinline void loop1D(Function const& function)const
		{
			for (size_t i = 0; i < size(); ++i)
			{
				function(i);
			}
		}

		template <class Function>
		__forceinline void loop2D(Function const& function)const
		{
			// TEST if a 1D loop is more efficient
			for (size_t minor = 0; minor < minorSize(); ++minor)
			{
				for (size_t major = 0; major < majorSize(); ++major)
				{
					if constexpr (M_ROW_MAJOR)
						function(major, minor);
					else
						function(minor, major);
				}
			}
		}

		constexpr T* data()
		{
			return (T*)rawData();
		}

		constexpr const T* data()const
		{
			return (const T*)rawData();
		}

		constexpr T const& operator[](size_t index)const
		{
			return data()[index];
		}

		constexpr T& operator[](size_t index)
		{
			return data()[index];
		}

		// i -- (column index)
		// j |  (line index)
		constexpr T const& operator()(size_t i, size_t j) const
		{
			assert(i >= 0);
			assert(j >= 0);
			assert(i < width());
			assert(j < height());
			return data()[index(i, j)];
		}

		// i -- (column index)
		// j |  (line index)
		constexpr T & operator()(size_t i, size_t j) 
		{
			assert(i >= 0);
			assert(j >= 0);
			assert(i < width());
			assert(j < height());
			return data()[index(i, j)];
		}
		
		constexpr __img::ImageContent<T, M_ROW_MAJOR> content()const
		{
			return __img::ImageContent<T, M_ROW_MAJOR>{ this };
		}
	};

}

template <class Stream>
Stream& operator<< (Stream & stream, img::FormatlessImage const& img)
{
	stream << "Image(w = " << img.width() << ", h = " << img.height() << ", buffer_size = " << img.byteSize() << ")";
	return stream;
}

template <class Stream, class T, bool ROW_MAJOR>
Stream& operator<<(Stream& stream, img::Image<T, ROW_MAJOR> const& img)
{
	stream << "Image<" << typeid(T).name() << ", " << (img.MAJOR() ? "ROW" : "COL") << ">(" << img.width() << ", " << img.height() << ")";
	return stream;
}

template <class Stream, class T, bool ROW_MAJOR>
Stream& operator<<(Stream& stream, img::__img::ImageContent<T, ROW_MAJOR> const& c)
{
	// Why do I have to do it like that
	const img::Image<T, ROW_MAJOR>& img = *(img::Image<T, ROW_MAJOR>*)(c.img);
	//stream << "{";
	for (int j = 0; j < img.height(); ++j)
	{
		for (int i = 0; i < img.width(); ++i)
		{
			stream << img(i, j);
			if(i != img.width()-1)
				stream << ", ";

		}
		if(j != img.height()-1)
			stream << '\n';
	}
	//stream << "}";
	return stream;
}

#undef me