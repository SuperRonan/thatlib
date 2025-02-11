#pragma once

#include <cstring>
#include <algorithm>
#include <type_traits>
#include <cassert>

#define me (*this)

namespace that
{

	namespace math
	{
		template <int N, class T>
		class Vector
		{
		protected:

			T m_data[N];

		public:

			using _Type = T;

			static constexpr int size() noexcept
			{
				return N;
			}

			constexpr Vector() noexcept
			{
				// This assumes a 0 of T is made of bytes 0
				std::memset(m_data, 0, N * sizeof(T));
			}

			template <class Q, class... Args>
			constexpr Vector(Q const& q, Args const& ... values) noexcept
			{
				constexpr int M = sizeof...(Args);
				if constexpr (M == 0)
				{
					std::fill(begin(), end(), q);
				}
				else
				{
					static_assert(M == N - 1, "Vector variadic constructor: the number of args should be the same as the size!");
					m_data[0] = q;
					int i = 1;
					((m_data[i++] = values), ...);
				}
			}
		
			template <class Q>
			constexpr Vector(Vector<N, Q> const& other) noexcept
			{
				if constexpr (std::is_same<T, Q>::value && std::is_trivially_copyable<T>::value)
				{
					std::memcpy(m_data, other.m_data, N * sizeof(T));
				}
				else
				{
					std::copy(other.cbegin(), other.cend(), begin());
				}
			}

			constexpr Vector(Vector const& other) noexcept
			{
				if constexpr (std::is_trivially_copyable<T>::value)
				{
					std::memcpy(m_data, other.m_data, N * sizeof(T));
				}
				else
				{
					std::copy(other.cbegin(), other.cend(), begin());
				}
			}

			template <class Q>
			constexpr Vector& operator=(Vector<N, Q> const& other) noexcept
			{
				if constexpr (std::is_same<T, Q>::value && std::is_trivially_copyable<T>::value)
				{
					std::memcpy(m_data, other.m_data, N * sizeof(T));
				}
				else
				{
					std::copy(other.cbegin(), other.cend(), begin());
				}
				return me;
			}

			constexpr Vector& operator=(Vector const& other) noexcept
			{
				if constexpr (std::is_trivially_copyable<T>::value)
				{
					std::memcpy(m_data, other.m_data, N * sizeof(T));
				}
				else
				{
					std::copy(other.cbegin(), other.cend(), begin());
				}
				return me;
			}

			constexpr T& operator[](int i) noexcept
			{
				assert(i >= 0);
				assert(i < N);
				return m_data[i];
			}

			constexpr T const& operator[](int i)const noexcept
			{
				assert(i >= 0);
				assert(i < N);
				return m_data[i];
			}

			constexpr T* begin() noexcept
			{
				return m_data;
			}

			constexpr T* end() noexcept
			{
				return m_data + N;
			}

			constexpr const T* begin()const noexcept
			{
				return m_data;
			}

			constexpr const T* end()const noexcept
			{
				return m_data + N;
			}

			constexpr const T* cbegin()const noexcept
			{
				return m_data;
			}

			constexpr const T* cend()const noexcept
			{
				return m_data + N;
			}

			template <class Q>
			constexpr Vector& operator+=(Vector<N, Q> const& other) noexcept
			{
				for (int i = 0; i < N; ++i)
				{
					me[i] += other[i];
				}
				return me;
			}

			template <class Q>
			constexpr Vector& operator-=(Vector<N, Q> const& other) noexcept
			{
				for (int i = 0; i < N; ++i)
				{
					me[i] -= other[i];
				}
				return me;
			}

			template <class Q>
			constexpr Vector& operator*=(Vector<N, Q> const& other) noexcept
			{
				for (int i = 0; i < N; ++i)
				{
					me[i] *= other[i];
				}
				return me;
			}

			template <class Q>
			constexpr Vector& operator/=(Vector<N, Q> const& other) noexcept
			{
				for (int i = 0; i < N; ++i)
				{
					me[i] /= other[i];
				}
				return me;
			}

			template <class Q>
			constexpr Vector operator+(Vector<N, Q> const& other)const noexcept
			{
				Vector res = me;
				res += other;
				return res;
			}

			template <class Q>
			constexpr Vector operator-(Vector<N, Q> const& other)const noexcept
			{
				Vector res = me;
				res -= other;
				return res;
			}

			template <class Q>
			constexpr Vector operator*(Vector<N, Q> const& other)const noexcept
			{
				Vector res = me;
				res *= other;
				return res;
			}

			template <class Q>
			constexpr Vector operator/(Vector<N, Q> const& other)const noexcept
			{
				Vector res = me;
				res /= other;
				return res;
			}

			constexpr Vector& operator*=(T const& t) noexcept
			{
				for (int i = 0; i < N; ++i)
				{
					me[i] *= t;
				}
				return me;
			}

			constexpr Vector& operator/=(T const& t) noexcept
			{
				for (int i = 0; i < N; ++i)
				{
					me[i] /= t;
				}
				return me;
			}

			constexpr Vector operator*(T const& t)const noexcept
			{
				Vector res = me;
				res *= t;
				return res;
			}

			constexpr Vector operator/(T const& t)const noexcept
			{
				Vector res = me;
				res /= t;
				return res;
			}

			template <typename std::enable_if<N == 1>>
			constexpr operator T()const noexcept
			{
				return me[0];
			}
		};

		template <int N, class T, class Q>
		constexpr T dot(Vector<N, T> const& t, Vector<N, Q> const& q) noexcept
		{
			T res(0);
			for (int i = 0; i < N; ++i)
				res += t[i] * q[i];
			return res;
		}

		template <typename _Ty>
		struct Is_Vector : std::false_type {};

		template<int N, class T>
		struct Is_Vector<Vector<N, T>> : std::true_type {};
	}

}

template <int N, class T, class Q>
constexpr inline that::math::Vector<N, T> operator*(Q const& q, that::math::Vector<N, T> const& vec) noexcept
{
	return vec * q;
}

template <int N, class T, class Q>
constexpr inline that::math::Vector<N, T> operator/(Q const& q, that::math::Vector<N, T> const& vec) noexcept
{
	that::math::Vector<N, T> left = q;
	return left / vec;
}

namespace std
{
	template <int N, class T>
	struct is_trivially_copyable < that::math::Vector < N, T >> : public is_trivially_copyable<T>::type {};
}

template <class Stream, int N, class T>
Stream& operator<<(Stream& stream, that::math::Vector<N, T> const& vec) 
{
	stream << "(";
	for (int i = 0; i < vec.size(); ++i)
	{
		if constexpr(std::_Is_character<T>::value)
			stream << (int)vec[i];
		else
			stream << vec[i];
		if (i != vec.size() - 1)
			stream << ", ";
	}
	stream << ')';
	return stream;
}

#undef me