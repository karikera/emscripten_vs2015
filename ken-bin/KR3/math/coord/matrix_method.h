#pragma once

#include "matrix_data.h"

namespace kr
{
	namespace math
	{
		template <typename T, size_t rows, size_t cols, bool aligned, matrix_data_type type>
		using matrix_constructor_auto = matrix_constructor<T, rows, cols, aligned, type, meta::types_n<vector<T, cols, aligned>, rows>, meta::types_n<T, rows*cols>>;

		template <typename T, size_t rows, size_t cols, bool aligned, matrix_data_type type> 
		class matrix_method
			:public matrix_constructor_auto<T, rows, cols, aligned, type>
		{
			using super = matrix_constructor_auto<T, rows, cols, aligned, type>;
		public:
			using method_t = matrix_method;
			using super::super;
			using super::v;
		};;

		template <typename T, size_t cols, bool aligned>
		class matrix_method<T, 2, cols, aligned, matrix_data_type::width_height>
			:public matrix_constructor_auto<T, 2, cols, aligned, matrix_data_type::width_height>
		{
			using super = matrix_constructor_auto<T, 2, cols, aligned, matrix_data_type::width_height>;
		public:
			using method_t = matrix_method;
			using super::super;
			using super::v;
			using super::pos;
			using super::size;

			ATTR_INLINE const vector<T, cols, aligned> clip(const vector<T, cols, aligned> & o) const noexcept;
			ATTR_INLINE bool contains(const vector<T, cols, aligned> & o) const noexcept;
			ATTR_INLINE bool intersect(const matrix<T, 2, cols, aligned, matrix_data_type::width_height> & rc) const noexcept;
			ATTR_INLINE bool getIntersect(
				matrix<T, 2, cols, aligned, matrix_data_type::width_height>  * _out, 
				const matrix<T, 2, cols, aligned, matrix_data_type::width_height> & rc) const noexcept;
		};;
		template <typename T, size_t cols, bool aligned>
		class matrix_method<T, 2, cols, aligned, matrix_data_type::right_bottom>
			:public matrix_constructor_auto<T, 2, cols, aligned, matrix_data_type::right_bottom>
		{
			using super = matrix_constructor_auto<T, 2, cols, aligned, matrix_data_type::right_bottom>;
		public:
			using method_t = matrix_method;
			using super::super;
			using super::v;
			using super::from;
			using super::to;

			ATTR_INLINE const vector<T, cols, aligned> clip(const vector<T, cols, aligned> & o) const noexcept;
			ATTR_INLINE bool contains(const vector<T, cols, aligned> & o) const noexcept;
			ATTR_INLINE bool intersect(const matrix<T, 2, cols, aligned, matrix_data_type::right_bottom> & rc) const noexcept;
			ATTR_INLINE bool getIntersect(
				matrix<T, 2, cols, aligned, matrix_data_type::right_bottom> * _out,
				const matrix<T, 2, cols, aligned, matrix_data_type::right_bottom> & rc) const noexcept;
		};;

		template <typename T, bool aligned>
		class matrix_method<T, 3, 3, aligned>
			:public matrix_constructor_auto<T, 3, 3, aligned, matrix_data_type::none>
		{
			using super = matrix_constructor_auto<T, 3, 3, aligned, matrix_data_type::none>;
		public:
			using method_t = matrix_method;
			using super::super;
			using super::v;
			ATTR_INLINE static const matrix<T, 3, 3, aligned> identity() noexcept;
		};;

		template <typename T, bool aligned>
		class matrix_method<T, 4, 4, aligned>
			:public matrix_constructor_auto<T, 4, 4, aligned, matrix_data_type::none>
		{
			using super = matrix_constructor_auto<T, 4, 4, aligned, matrix_data_type::none>;
		public:
			using method_t = matrix_method;
			using super::super;
			using super::v;
			using super::_11;
			using super::_12;
			using super::_13;
			using super::_14;
			using super::_21;
			using super::_22;
			using super::_23;
			using super::_24;
			using super::_31;
			using super::_32;
			using super::_33;
			using super::_34;
			using super::_41;
			using super::_42;
			using super::_43;
			using super::_44;

			ATTR_INLINE const vector<T, 4, aligned> getX() const noexcept;
			ATTR_INLINE const vector<T, 4, aligned> getY() const noexcept;
			ATTR_INLINE const vector<T, 4, aligned> getZ() const noexcept;
			ATTR_INLINE const vector<T, 4, aligned> getPos() const noexcept;

			inline const matrix<T, 4, 4, aligned> inverse() const noexcept;
			ATTR_INLINE void scaling(T scale) noexcept;
			ATTR_INLINE void preScale(const vector<T, 4, aligned> & scale) noexcept;
			ATTR_INLINE void postScale(const vector<T, 4, aligned> & scale) noexcept;

			ATTR_INLINE static const matrix<T, 4, 4, aligned> world(const vector<T, 4, aligned> &pos, const vector<T, 4, aligned> &forward, const vector<T, 4, aligned> &up) noexcept;
			ATTR_INLINE static const matrix<T, 4, 4, aligned> look(const vector<T, 4, aligned> &eye, const vector<T, 4, aligned> &forward, const vector<T, 4, aligned> &up) noexcept;
			ATTR_INLINE static const matrix<T, 4, 4, aligned> lookAt(const vector<T, 4, aligned> &eye, const vector<T, 4, aligned> &at, const vector<T, 4, aligned> &up) noexcept;
			ATTR_INLINE static const matrix<T, 4, 4, aligned> translate(vector<T, 3> pos) noexcept;
			ATTR_INLINE static const matrix<T, 4, 4, aligned> identity() noexcept;
			ATTR_INLINE static const matrix<T, 4, 4, aligned> scale(const vector<T, 4, aligned> &scale) noexcept;
			ATTR_INLINE static const matrix<T, 4, 4, aligned> scale(T x) noexcept;
			ATTR_INLINE static const matrix<T, 4, 4, aligned> rotateX(T rad) noexcept;
			ATTR_INLINE static const matrix<T, 4, 4, aligned> rotateY(T rad) noexcept;
			ATTR_INLINE static const matrix<T, 4, 4, aligned> rotateZ(T rad) noexcept;
			ATTR_INLINE static const matrix<T, 4, 4, aligned> rotateAxis(const vector<T, 4, aligned> & pos, T rad) noexcept;

			ATTR_INLINE static const matrix<T, 4, 4, aligned> translate(T x, T y, T z) noexcept
			{
				return translate(vector<T, 3>(x, y, z));
			}
			ATTR_INLINE static const matrix<T, 4, 4, aligned> scale(T x, T y, T z, T w = 1.f) noexcept
			{
				return scale(vector<T, 4, aligned>(x, y, z, w));
			}

			ATTR_INLINE static const matrix<T, 4, 4, aligned> orthogonal(T width, T height) noexcept;
			ATTR_INLINE static const matrix<T, 4, 4, aligned> orthogonal(T left, T top, T right, T bottom, T _near, T _far) noexcept;
			ATTR_INLINE static const matrix<T, 4, 4, aligned> fov(T fovy, T aspect, T near, T far) noexcept;


		};;
		
		template <> 
		ATTR_INLINE const mat4a matrix_method<float, 4, 4, true>::inverse() const noexcept;
		template <> 
		ATTR_INLINE void matrix_method<float, 4, 4, true>::scaling(float scale) noexcept;
		template <> 
		ATTR_INLINE void matrix_method<float, 4, 4, true>::postScale(const vec4a & scale) noexcept;

		template <typename T, size_t cols> 
		ATTR_INLINE const regionwh<T, cols> intersect(const regionwh<T, cols>& a, const regionwh<T, cols>& b) noexcept;
		template <typename T, size_t cols>
		ATTR_INLINE const region<T, cols> intersect(const region<T, cols>& a, const region<T, cols>& b) noexcept;

		template <>
		ATTR_INLINE const mat4a matrix_method<float, 4, 4, true>::inverse() const noexcept;
		template <>
		ATTR_INLINE void matrix_method<float, 4, 4, true>::scaling(float scale) noexcept;
		template <>
		ATTR_INLINE void matrix_method<float, 4, 4, true>::postScale(const vec4a & scale) noexcept;
		template <>
		ATTR_INLINE const mat4a matrix_method<float, 4, 4, true>::world(const vec4a &pos, const vec4a &forward, const vec4a &up) noexcept;
		template <>
		ATTR_INLINE const mat4a matrix_method<float, 4, 4, true>::look(const vec4a &eye, const vec4a &forward, const vec4a &up) noexcept;
		template <>
		ATTR_INLINE const mat4a matrix_method<float, 4, 4, true>::lookAt(const vec4a &eye, const vec4a &to, const vec4a &up) noexcept;
		template <>
		ATTR_INLINE const mat4a matrix_method<float, 4, 4, true>::translate(vector<float, 3> pos) noexcept;
		template <>
		ATTR_INLINE const mat4a matrix_method<float, 4, 4, true>::identity() noexcept;
		template <>
		ATTR_INLINE const mat4a matrix_method<float, 4, 4, true>::scale(const vec4a &scale) noexcept;
		template <>
		ATTR_INLINE const mat4a matrix_method<float, 4, 4, true>::scale(float x) noexcept;
		template <>
		ATTR_INLINE const mat4a matrix_method<float, 4, 4, true>::rotateX(float rad) noexcept;
		template <>
		ATTR_INLINE const mat4a matrix_method<float, 4, 4, true>::rotateY(float rad) noexcept;
		template <>
		ATTR_INLINE const mat4a matrix_method<float, 4, 4, true>::rotateZ(float rad) noexcept;
		template <>
		ATTR_INLINE const mat4a matrix_method<float, 4, 4, true>::orthogonal(float width, float height) noexcept;
		template <>
		ATTR_INLINE const mat4a matrix_method<float, 4, 4, true>::orthogonal(float left, float top, float right, float bottom, float _near, float _far) noexcept;
		template <>
		ATTR_INLINE const mat4a matrix_method<float, 4, 4, true>::fov(float fovy, float aspect, float _near, float _far) noexcept;
	}
}

using kr::math::intersect;
