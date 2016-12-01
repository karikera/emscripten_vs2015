#pragma once

#include "matrix.h"

namespace kr
{
	namespace math
	{
		template <typename T, size_t rows, size_t cols, bool aligned, matrix_data_type type>
		ATTR_INLINE vector<T, cols, aligned>& matrix<T, rows, cols, aligned, type>::operator [](size_t idx) noexcept
		{
			return v[idx];
		}
		template <typename T, size_t rows, size_t cols, bool aligned, matrix_data_type type>
		ATTR_INLINE const vector<T, cols, aligned>& matrix<T, rows, cols, aligned, type>::operator [](size_t idx) const noexcept
		{
			return v[idx];
		}
		template <typename T, size_t rows, size_t cols, bool aligned, matrix_data_type type>
		inline bool matrix<T, rows, cols, aligned, type>::operator ==(const matrix<T, rows, cols, aligned, type> & m) const noexcept
		{
			return mem::equals(this, &m, sizeof(m));
		}
		template <typename T, size_t rows, size_t cols, bool aligned, matrix_data_type type>
		inline bool matrix<T, rows, cols, aligned, type>::operator !=(const matrix<T, rows, cols, aligned, type> & m) const noexcept
		{
			return !mem::equals(this, &m, sizeof(m));
		}
		template <typename T, size_t rows, size_t cols, bool aligned, matrix_data_type type>
		inline const matrix<T, rows, cols, aligned, type> matrix<T, rows, cols, aligned, type>::operator *(const matrix<T, rows, cols, aligned, type> & m) const noexcept
		{
			static_assert(rows == cols, "mismatch count ");
			matrix<T, rows, cols, aligned, type> out;
			constexpr size_t others_cols = rows;

			for (size_t i = 0; i<rows; i++)
			{
				for (size_t j = 0; j<others_cols; j++)
				{
					T sum = v[i][0] * m.v[0][j];
					for (size_t k = 1; k < cols; k++)
					{
						sum += v[i][k] * m.v[k][j];
					}
					out.v[i].m_data[j] = sum;
				}
			}
			return out;
		}
		template <typename T, size_t rows, size_t cols, bool aligned, matrix_data_type type>
		inline const vector<T, rows, aligned> matrix<T, rows, cols, aligned, type>::operator *(const vector<T, cols, aligned> & o) const noexcept
		{
			vector<T, rows> out;
			for (size_t i = 0; i<rows; i++)
			{
				T sum = v[i][0] * o[0];
				for (size_t k = 1; k < cols; k++)
				{
					sum += v[i][k] * o[k];
				}
				out.m_data[i] = sum;
			}
			return out;
		}
		template <typename T, size_t rows, size_t cols, bool aligned, matrix_data_type type>
		ATTR_INLINE matrix<T, rows, cols, aligned, type>& matrix<T, rows, cols, aligned, type>::operator *=(const matrix<T, rows, cols, aligned, type> & m) noexcept
		{
			return *this = *this * m;
		}
		template <typename T, size_t rows, size_t cols, bool aligned, matrix_data_type type>
		inline matrix<T, rows, cols, aligned, type>& matrix<T, rows, cols, aligned, type>::operator +=(const matrix<T, rows, cols, aligned, type> & m) noexcept
		{
			const T* src = (T*)(&m);
			T* dst = (T*)(this);
			T* dstend = dst + rows*cols;

			while (dst != dstend)
			{
				*dst++ += *src++;
			}
			return *this;
		}
		template <typename T, size_t rows, size_t cols, bool aligned, matrix_data_type type>
		inline const matrix<T, rows, cols, aligned, type> matrix<T, rows, cols, aligned, type>::operator +(const matrix<T, rows, cols, aligned, type> & m) const noexcept
		{
			matrix<T, rows, cols, aligned, type> out;
			const T* src1 = (T*)(this);
			const T* src2 = (T*)(&m);
			T* dst = (T*)(&out);
			T* dstend = dst + rows*cols;

			while (dst != dstend)
			{
				*dst++ = *src1++ + *src2++;
			}
			return out;
		}
		template <typename T, size_t rows, size_t cols, bool aligned, matrix_data_type type>
		inline matrix<T, rows, cols, aligned, type>& matrix<T, rows, cols, aligned, type>::operator -=(const matrix<T, rows, cols, aligned, type> & m) noexcept
		{
			const T* src = (T*)(&m);
			T* dst = (T*)(this);
			T* dstend = dst + rows*cols;

			while (dst != dstend)
			{
				*dst++ -= *src++;
			}
			return *this;
		}
		template <typename T, size_t rows, size_t cols, bool aligned, matrix_data_type type>
		inline const matrix<T, rows, cols, aligned, type> matrix<T, rows, cols, aligned, type>::operator -(const matrix<T, rows, cols, aligned, type> & m) const noexcept
		{
			matrix<T, rows, cols, aligned, type> out;
			const T* src1 = (T*)(this);
			const T* src2 = (T*)(&m);
			T* dst = (T*)(&out);
			T* dstend = dst + rows*cols;

			while (dst != dstend)
			{
				*dst++ = *src1++ - *src2++;
			}
			return out;
		}
		template <typename T, size_t rows, size_t cols, bool aligned, matrix_data_type type>
		inline bool matrix<T, rows, cols, aligned, type>::similar(const matrix<T, rows, cols, aligned, type> & o) const noexcept
		{
			const float almostZero = 1.0e-6f;

			const vector<T, cols, aligned> * dest = v;
			const vector<T, cols, aligned> * src = o.v;
			const vector<T, cols, aligned> * end = dest + rows;
			T dist = (*src - *dest).length_sq();
			while (dest != end)
			{
				dist += (*src - *dest).length_sq();
				dest++;
				src++;
			}
			return dist <= almostZero;
		}
		template <typename T, size_t rows, size_t cols, bool aligned, matrix_data_type type>
		inline const matrix<T, cols, rows, aligned, type> matrix<T, rows, cols, aligned, type>::transpose() const noexcept
		{
			matrix<T, cols, rows, type> out;

			float * dest = (float*)(&out);
			const float * src = (float*)(&v);
			for (size_t r = 0; r < rows; r++)
			{
				for (size_t c = 0; c < cols; c++)
				{
					dest[r + c*rows] = src[c + r*cols];
				}
			}

			return out;
		}
		template <typename T, size_t rows, size_t cols, bool aligned, matrix_data_type type>
		template <int ... args>
		inline const matrix<T, rows, cols, aligned, type> matrix<T, rows, cols, aligned, type>::__shuffle() const noexcept
		{
			static_assert(sizeof ... (args) == cols*rows, "size unmatch");
			matrix<T, rows, cols, aligned, type> out;
			auto values = { args ... };
			T * dest = (T*)(&out);
			T * src = (T*)(v);

			for (int v : values) *dest++ = src[v];
			return out;
		}
	}
}

#include "mmx.h"

namespace kr
{
	namespace math
	{

		template <>
		inline const mat4a mat4a::operator *(const mat4a & m) const noexcept
		{
			mat4a out;
			const vec4a * a = v;
			const vec4a * bstart = &m.v[0];
			vec4a * r = &out.v[0];
			for (size_t i = 0; i<4; i++)
			{
				const vec4a * b = bstart;
				vec4a res = a->getXV() * *b;
				b++;
				res += a->getYV() * *b;
				b++;
				res += a->getZV() * *b;
				b++;
				res += a->getWV() * *b;

				*r = res;
				r++;
				a++;
			}
			return out;
		}
		template <>
		inline const vec4a mat4a::operator *(const vec4a & o) const noexcept
		{
			vec4a out;
			const vec4a * a = &v[0];
			out.x = dot(o, *a++);
			out.y = dot(o, *a++);
			out.z = dot(o, *a++);
			out.w = dot(o, *a++);
			return out;
		}

		template <>
		inline const mat4a mat4a::transpose() const noexcept
		{
			// x.x,x.y,y.x,y.y
			vec4a vTemp1 = shuffle<0, 1, 0, 1>(v[0], v[1]);
			// x.z,x.w,y.z,y.w
			vec4a vTemp3 = shuffle<2, 3, 2, 3>(v[0], v[1]);
			// z.x,z.y,w.x,w.y
			vec4a vTemp2 = shuffle<0, 1, 0, 1>(v[2], v[3]);
			// z.z,z.w,w.z,w.w
			vec4a vTemp4 = shuffle<2, 3, 2, 3>(v[2], v[3]);

			mat4a mResult;
			// x.x,y.x,z.x,w.x
			mResult[0] = shuffle<0, 2, 0, 2>(vTemp1, vTemp2);
			// x.y,y.y,z.y,w.y
			mResult[1] = shuffle<1, 3, 1, 3>(vTemp1, vTemp2);
			// x.z,y.z,z.z,w.z
			mResult[2] = shuffle<0, 2, 0, 2>(vTemp3, vTemp4);
			// x.w,y.w,z.w,w.w
			mResult[3] = shuffle<1, 3, 1, 3>(vTemp3, vTemp4);
			return mResult;
		}
	}
}
