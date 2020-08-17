#pragma once

#include <cassert>
#include <type_traits>
#include <algorithm>
#include <typeinfo>
#include <cassert>

#include "../math/Vector.h"

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
	uint index(uint i, uint j, uint width, uint height) noexcept
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

	// It is easier to move on major
	template <class T, bool M_ROW_MAJOR = IMAGE_ROW_MAJOR>
	class Image
	{
	protected:

		using sint = signed long long;
		using uint = unsigned long long;

		// If the image is empty, m_size == 0
		uint m_width, m_height, m_size;

		// If the image is empty, m_data == nulltpr
		T* m_data;

		template <class Q, bool _ROW_MAJOR>
		friend class Image;

		template<class Q, bool _ROW_MAJOR>
		void copyData(Image<Q, _ROW_MAJOR>const& other)
		{
			if constexpr (M_ROW_MAJOR == _ROW_MAJOR) // likely
			{
				if constexpr (std::is_same<T, Q>::value && std::is_trivially_copyable<T>::value)
					std::memcpy(m_data, other.m_data, bufferByteSize());
				else
					std::copy(other.cbegin(), other.cend(), begin());
			}
			else
			{
				loop2D([this, &other](uint i, uint j)
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

		constexpr Image(uint width = 0, uint height = 0) :
			m_width(width),
			m_height(height),
			m_size(width* height),
			m_data(m_size ? new T[m_size] : nullptr)
		{}

		constexpr Image(uint width, uint height, T const& default_value) :
			m_width(width),
			m_height(height),
			m_size(width* height),
			m_data(m_size ? new T[m_size] : nullptr)
		{
			if (m_data)
			{
				std::fill(begin(), end(), default_value);
			}
		}

		~Image()
		{
			if (!empty())
			{
				delete[] m_data;
				std::memset(this, 0, sizeof Image);
			}
		}

		template<class Q, bool _ROW_MAJOR>
		constexpr Image(Image<Q, _ROW_MAJOR> const& other) :
			m_width(other.width()),
			m_height(other.width()),
			m_size(other.size())
		{
			if (m_size)
			{
				m_data = new T[m_size];
				copyData(other);
			}
			else
			{
				m_data = nullptr;
			}
		}

		constexpr Image(Image const& other) :
			m_width(other.width()),
			m_height(other.width()),
			m_size(other.size())
		{
			if (m_size)
			{
				m_data = new T[m_size];
				copyData(other);
			}
			else
			{
				m_data = nullptr;
			}
		}

		// TODO template
		constexpr Image(Image && other) noexcept:
			m_width(other.width()),
			m_height(other.height()),
			m_size(other.size()),
			m_data(other.m_data) // steal the data
		{
			if (m_size)
			{
				other.m_data = nullptr;
				other.m_size = 0;
			}
			else
			{
				assert(empty());
			}
		}

		template<class Q, bool _ROW_MAJOR>
		constexpr Image& operator=(Image<Q, _ROW_MAJOR> const& other)
		{
			if (empty())
			{
				m_width = other.width();
				m_height = other.width();
				m_size = other.size();
				m_data = new T[size()];
			}
			else
			{
				if (size() == other.size()) // likely
				{
					// no need to reallocate memory
					m_width = other.width();
					m_height = other.height();
				}
				else // reallocate memory
				{
					// for the future, maybe keep the buffer is the new size is smaller
					delete[] m_data;
					m_width = other.width();
					m_height = other.width();
					m_size = other.size();
					m_data = new T[size()];
				}
			}
			copyData(other);
			return me;
		}

		constexpr Image& operator=(Image const& other) noexcept
		{
			if (empty())
			{
				m_width = other.width();
				m_height = other.width();
				m_size = other.size();
				m_data = new T[size()];
			}
			else
			{
				if (size() == other.size()) // likely
				{
					// no need to reallocate memory
					m_width = other.width();
					m_height = other.height();
				}
				else // reallocate memory
				{
					// for the future, maybe keep the buffer is the new size is smaller
					delete[] m_data;
					m_width = other.width();
					m_height = other.width();
					m_size = other.size();
					m_data = new T[size()];
				}
			}
			copyData(other);
			return me;
		}

		// TODO template
		constexpr Image& operator=(Image&& other) noexcept
		{
			if (!empty())
			{
				delete[] m_data;
			}
			std::memcpy(this, &other, sizeof Image);
			std::memset(&other, 0, sizeof Image);
			return me;
		}


		constexpr uint bufferByteSize()const
		{
			return m_size * sizeof(T);
		}

		constexpr bool empty()const
		{
			return m_data == nullptr;
		}

		constexpr T* begin()
		{
			return m_data;
		}

		constexpr T* end()
		{
			return m_data + size();
		}

		constexpr const T* begin()const
		{
			return m_data;
		}

		constexpr const T* end()const
		{
			return m_data + size();
		}

		constexpr const T* cbegin()const
		{
			return m_data;
		}

		constexpr const T* cend()const
		{
			return m_data + size();
		}

		// i -- (column index)
		// j |  (line index)
		constexpr uint index(uint i, uint j)const noexcept
		{
			return relativeIndex(i, j);
		}

		// i -- (column index)
		// j |  (line index)
		constexpr uint relativeIndex(uint i, uint j)const noexcept
		{
			return img::index<M_ROW_MAJOR, uint>(i, j, m_width, m_height);
		}

		constexpr uint fixedIndex(uint major_index, uint minor_index)const noexcept
		{
			return major_index + majorSize() * minor_index;
		}

		constexpr T const& fixedAccess(uint major_index, uint minor_index) const
		{
			assert(major_index >= 0);
			assert(minor_index >= 0);
			assert(major_index < majorSize());
			assert(minor_index < minorSize());
			return me[fixedIndex(major_index, minor_index)];
		}

		constexpr uint width()const noexcept
		{
			return m_width;
		}

		constexpr uint height()const noexcept
		{
			return m_height;
		}

		constexpr uint size()const noexcept
		{
			return m_size;
		}

		constexpr uint majorSize()const
		{
			if constexpr (M_ROW_MAJOR)	return m_width;
			else						return m_height;
		}

		constexpr uint minorSize()const
		{
			if constexpr (M_ROW_MAJOR)	return m_height;
			else						return m_width;
		}

		template <class Function>
		__forceinline void loop1D(Function const& function)const
		{
			for (uint i = 0; i < m_size; ++i)
			{
				function(i);
			}
		}

		template <class Function>
		__forceinline void loop2D(Function const& function)const
		{
			// TEST if a 1D loop is more efficient
			for (uint minor = 0; minor < minorSize(); ++minor)
			{
				for (uint major = 0; major < majorSize(); ++major)
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
			return m_data;
		}

		constexpr const T* data()const
		{
			return m_data;
		}

		constexpr T const& operator[](uint index)const
		{
			return m_data[index];
		}

		constexpr T& operator[](uint index)
		{
			return m_data[index];
		}

		// i -- (column index)
		// j |  (line index)
		constexpr T const& operator()(uint i, uint j) const
		{
			assert(i >= 0);
			assert(j >= 0);
			assert(i < width());
			assert(j < height());
			return m_data[index(i, j)];
		}

		// i -- (column index)
		// j |  (line index)
		constexpr T & operator()(uint i, uint j) 
		{
			assert(i >= 0);
			assert(j >= 0);
			assert(i < width());
			assert(j < height());
			return m_data[index(i, j)];
		}
		

		// Assumes data is of the right size
		template <class Q>
		constexpr void setData(Q* data)noexcept
		{
			if (!empty())
				delete[] m_data;
			m_data = (T*)data;
		}
		
		
		
		constexpr __img::ImageContent<T, M_ROW_MAJOR> content()const
		{
			return __img::ImageContent<T, M_ROW_MAJOR>{ this };
		}
	};

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