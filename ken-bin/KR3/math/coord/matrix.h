#pragma once

#include "matrix_method.h"
#include <iostream>

namespace kr
{
	namespace math
	{
		template <typename T, size_t rows, size_t cols, bool aligned, matrix_data_type type>
		class matrix
			:public matrix_method<T, rows, cols, aligned, type>
		{
		public:
			using matrix_method<T, rows, cols, aligned, type>::matrix_method;
			using matrix_method<T, rows, cols, aligned, type>::v;

			ATTR_INLINE vector<T, cols, aligned>& operator [](size_t idx) noexcept;
			ATTR_INLINE const vector<T, cols, aligned>& operator [](size_t idx) const noexcept;
			inline bool operator ==(const matrix & m) const noexcept;
			inline bool operator !=(const matrix & m) const noexcept;
			ATTR_INLINE matrix & operator *= (const matrix & m) noexcept;
			inline const matrix operator *(const matrix & m) const noexcept;
			inline matrix & operator += (const matrix & m) noexcept;
			inline const matrix operator +(const matrix & m) const noexcept;
			inline matrix & operator -= (const matrix & m) noexcept;
			inline const matrix operator -(const matrix & m) const noexcept;
			inline const vector<T, rows, aligned> operator *(const vector<T, cols, aligned> & o) const noexcept;

			inline const matrix<T, cols, rows, aligned, type> transpose() const noexcept;
			inline bool similar(const matrix & v) const noexcept;

		private:
			template <int ... args> 
			inline const matrix __shuffle() const noexcept;
		};

		template <> const mat4a mat4a::operator *(const mat4a & m) const noexcept;
		template <> const vec4a mat4a::operator *(const vec4a & o) const noexcept;
		template <> const mat4a mat4a::transpose() const noexcept;
	}
}
