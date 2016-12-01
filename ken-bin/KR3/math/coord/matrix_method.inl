#pragma once

#include "matrix_method.h"

namespace kr
{
	namespace math
	{
		template <typename T, size_t cols, bool aligned>
		ATTR_INLINE const vector<T, cols, aligned> matrix_method<T, 2, cols, aligned, matrix_data_type::width_height>::clip(const vector<T, cols, aligned> & o) const noexcept
		{
			vector<T, cols, aligned> out;
			for (size_t c = 0; c < cols; c++)
			{
				if (pos[c] > o[c])
				{
					out[c] = pos[c];
					continue;
				}
				T end = pos[c] + size[c];
				if (end < o[c])
				{
					out[c] = end;
					continue;
				}
				out[c] = o[c];
			}
			return out;
		}
		template <typename T, size_t cols, bool aligned>
		ATTR_INLINE bool matrix_method<T, 2, cols, aligned, matrix_data_type::width_height>::contains(const vector<T, cols, aligned> & o) const noexcept
		{
			for (size_t c = 0; c < cols; c++)
			{
				if (pos[c] > o[c] || o[c] >= pos[c] + size[c]) return false;
			}
			return true;
		}
		template <typename T, size_t cols, bool aligned>
		ATTR_INLINE bool matrix_method<T, 2, cols, aligned, matrix_data_type::width_height>::getIntersect(
			matrix<T, 2, cols, aligned, matrix_data_type::width_height> * _out,
			const matrix<T, 2, cols, aligned, matrix_data_type::width_height> & rc) const noexcept
		{
			for (size_t c = 0; c < cols; c++)
			{
				T from = math::max(pos[c], rc.pos[c]);
				T to = math::min(pos[c] + size[c], rc.pos[c] + rc.size[c]);
				if (to <= from)
					return false;
				_out->pos[c] = from;
				_out->size[c] = to - from;
			}
			return true;
		}
		template <typename T, size_t cols, bool aligned>
		ATTR_INLINE bool matrix_method<T, 2, cols, aligned, matrix_data_type::width_height>::intersect(const matrix<T, 2, cols, aligned, matrix_data_type::width_height> & rc) const noexcept
		{
			for (size_t c = 0; c < cols; c++)
			{
				if (pos[c] <= rc.pos[c] + rc.size[c]) return false;
				if (rc.pos[c] <= pos[c] + size[c]) return false;
			}
			return true;
		}
		template <typename T, size_t cols, bool aligned>
		ATTR_INLINE const vector<T, cols, aligned> matrix_method<T, 2, cols, aligned, matrix_data_type::right_bottom>::clip(const vector<T, cols, aligned> & o) const noexcept
		{
			vector<T, cols, aligned> out;
			for (size_t c = 0; c < cols; c++)
			{
				if (from[c] > o[c])
				{
					out[c] = from[c];
					continue;
				}
				if (to[c] < o[c])
				{
					out[c] = to[c];
					continue;
				}
				out[c] = o[c];
			}
			return out;
		}
		template <typename T, size_t cols, bool aligned>
		ATTR_INLINE bool matrix_method<T, 2, cols, aligned, matrix_data_type::right_bottom>::contains(const vector<T, cols, aligned> & o) const noexcept
		{
			for (size_t c = 0; c < cols; c++)
			{
				if (from[c] > o[c]) return false;
				if (o[c] >= to[c]) return false;
			}
			return true;
		}
		template <typename T, size_t cols, bool aligned>
		ATTR_INLINE bool matrix_method<T, 2, cols, aligned, matrix_data_type::right_bottom>::getIntersect(
			matrix<T, 2, cols, aligned, matrix_data_type::right_bottom> * _out,
			const matrix<T, 2, cols, aligned, matrix_data_type::right_bottom> & rc) const noexcept
		{
			for (size_t c = 0; c < cols; c++)
			{
				T f = math::max(from[c], rc.from[c]);
				T t = math::min(to[c], rc.to[c]);
				if (t <= f)
					return false;
				_out->from[c] = f;
				_out->to[c] = t;
			}
			return true;
		}
		template <typename T, size_t cols, bool aligned>
		ATTR_INLINE bool matrix_method<T, 2, cols, aligned, matrix_data_type::right_bottom>::intersect(const matrix<T, 2, cols, aligned, matrix_data_type::right_bottom> & rc) const noexcept
		{
			for (size_t c = 0; c < cols; c++)
			{
				if (from[c] <= rc.to[c]) return false;
				if (rc.from[c] <= to[c]) return false;
			}
			return true;
		}

		template <typename T, bool aligned>
		ATTR_INLINE const matrix<T, 3, 3, aligned> matrix_method<T, 3, 3, aligned>::identity() noexcept
		{
			return{
				{ 1,0,0 },
				{ 0,1,0 },
				{ 0,0,1 } };
		}

		template <typename T, bool aligned>
		ATTR_INLINE const vector<T, 4, aligned> matrix_method<T, 4, 4, aligned>::getX() const noexcept
		{
			return{ v[0][0],v[1][0],v[2][0],v[3][0] };
		}
		template <typename T, bool aligned>
		ATTR_INLINE const vector<T, 4, aligned> matrix_method<T, 4, 4, aligned>::getY() const noexcept
		{
			return{ v[0][1],v[1][1],v[2][1],v[3][1] };
		}
		template <typename T, bool aligned>
		ATTR_INLINE const vector<T, 4, aligned> matrix_method<T, 4, 4, aligned>::getZ() const noexcept
		{
			return{ v[0][2],v[1][2],v[2][2],v[3][2] };
		}
		template <typename T, bool aligned>
		ATTR_INLINE const vector<T, 4, aligned> matrix_method<T, 4, 4, aligned>::getPos() const noexcept
		{
			return{ v[0][3],v[1][3],v[2][3],v[3][3] };
		}
		template <typename T, bool aligned>
		inline const matrix<T, 4, 4, aligned> matrix_method<T, 4, 4, aligned>::inverse() const noexcept
		{
			_assert(!"TODO: implement");
			return matrix<T, 4, 4, aligned>();
		}
		template <typename T, bool aligned>
		ATTR_INLINE void matrix_method<T, 4, 4, aligned>::scaling(T scale) noexcept
		{
			_11 *= scale;
			_12 *= scale;
			_13 *= scale;
			_21 *= scale;
			_22 *= scale;
			_23 *= scale;
			_31 *= scale;
			_32 *= scale;
			_33 *= scale;
		}
		template <typename T, bool aligned>
		ATTR_INLINE void matrix_method<T, 4, 4, aligned>::preScale(const vector<T, 4, aligned> & scale) noexcept
		{
			v[0] *= scale;
			v[1] *= scale;
			v[2] *= scale;
			v[3] *= scale;
		}
		template <typename T, bool aligned>
		ATTR_INLINE void matrix_method<T, 4, 4, aligned>::postScale(const vector<T, 4, aligned> & scale) noexcept
		{
			v[0] *= scale.x;
			v[1] *= scale.y;
			v[2] *= scale.z;
			v[3] *= scale.w;
		}
		template <typename T, bool aligned>
		ATTR_INLINE const matrix<T, 4, 4, aligned> matrix_method<T, 4, 4, aligned>::world(const vector<T, 4, aligned> &pos, const vector<T, 4, aligned> &forward, const vector<T, 4, aligned> &up) noexcept
		{
			vector<T, 4, aligned> z = -forward.normalize();  // Forward
			vector<T, 4, aligned> x = cross(up, z).normalize(); // Right
			vector<T, 4, aligned> y = cross(z, x);

			return{
				{ x.x, y.x, z.x, pos.x },
				{ x.y, y.y, z.y, pos.y },
				{ x.z, y.z, z.z, pos.z },
				{ 0, 0, 0, 1 }
			};
		}
		template <typename T, bool aligned>
		ATTR_INLINE const matrix<T, 4, 4, aligned> matrix_method<T, 4, 4, aligned>::look(const vector<T, 4, aligned> &eye, const vector<T, 4, aligned> &forward, const vector<T, 4, aligned> &up) noexcept
		{
			vector<T, 4, aligned> z = -forward.normalize();  // Forward
			vector<T, 4, aligned> x = cross(up, z).normalize(); // Right
			vector<T, 4, aligned> y = cross(z, x);

			return{
				{ x.x, x.y, x.z, (T)-dot(x,eye) },
				{ y.x, y.y, y.z, (T)-dot(y,eye) },
				{ z.x, z.y, z.z, (T)-dot(z,eye) },
				{ 0, 0, 0, 1 }
			};
		}
		template <typename T, bool aligned>
		ATTR_INLINE const matrix<T, 4, 4, aligned> matrix_method<T, 4, 4, aligned>::lookAt(const vector<T, 4, aligned> &eye, const vector<T, 4, aligned> &to, const vector<T, 4, aligned> &up) noexcept
		{
			return look(eye, to - eye, up);
		}
		template <typename T, bool aligned>
		ATTR_INLINE const matrix<T, 4, 4, aligned> matrix_method<T, 4, 4, aligned>::translate(vector<T, 3> pos) noexcept
		{
			return{
				{ 1,0,0,pos.x },
				{ 0,1,0,pos.y },
				{ 0,0,1,pos.z },
				{ 0,0,0,1 }
			};
		}
		template <typename T, bool aligned>
		ATTR_INLINE const matrix<T, 4, 4, aligned> matrix_method<T, 4, 4, aligned>::identity() noexcept
		{
			return{
				{ 1,0,0,0 },
				{ 0,1,0,0 },
				{ 0,0,1,0 },
				{ 0,0,0,1 }
			};
		}
		template <typename T, bool aligned>
		ATTR_INLINE const matrix<T, 4, 4, aligned> matrix_method<T, 4, 4, aligned>::scale(const vector<T, 4, aligned> &scale) noexcept
		{
			return{
				{ scale.x,0,0,0 },
				{ 0,scale.y,0,0 },
				{ 0,0,scale.z,0 },
				{ 0,0,0,1 }
			};
		}
		template <typename T, bool aligned>
		ATTR_INLINE const matrix<T, 4, 4, aligned> matrix_method<T, 4, 4, aligned>::scale(T x) noexcept
		{
			return scale(vector<T, 4, aligned>(x, x, x, 1));
		}
		template <typename T, bool aligned>
		ATTR_INLINE const matrix<T, 4, 4, aligned> matrix_method<T, 4, 4, aligned>::rotateX(T rad) noexcept
		{
			T c = (T)math::cos((float)rad);
			T s = (T)math::sin((float)rad);
			return{
				{ 1,0,0,0 },
				{ 0,c,(T)-s,0 },
				{ 0,s,c,0 },
				{ 0,0,0,1 }
			};
		}
		template <typename T, bool aligned>
		ATTR_INLINE const matrix<T, 4, 4, aligned> matrix_method<T, 4, 4, aligned>::rotateY(T rad) noexcept
		{
			T c = (T)math::cos((float)rad);
			T s = (T)math::sin((float)rad);
			return{
				{ c,0,s,0 },
				{ 0,1,0,0 },
				{ (T)-s,0,c,0 },
				{ 0,0,0,1 }
			};
		}
		template <typename T, bool aligned>
		ATTR_INLINE const matrix<T, 4, 4, aligned> matrix_method<T, 4, 4, aligned>::rotateZ(T rad) noexcept
		{
			T c = (T)math::cos((float)rad);
			T s = (T)math::sin((float)rad);
			return{
				{ c,(T)-s,0,0 },
				{ s,c,0,0 },
				{ 0,0,1,0 },
				{ 0,0,0,1 }
			};
		}
		template <typename T, bool aligned>
		ATTR_INLINE const matrix<T, 4, 4, aligned> matrix_method<T, 4, 4, aligned>::rotateAxis(const vector<T, 4, aligned> & pos, T rad) noexcept
		{
			return (matrix<T, 4, 4, aligned>)quaterniona::rotateAxis((vec4a)pos, (float)rad).toMatrix();
		}
		template <typename T, bool aligned>
		ATTR_INLINE const matrix<T, 4, 4, aligned> matrix_method<T, 4, 4, aligned>::orthogonal(T width, T height) noexcept
		{
			T _2w = 2 / width;
			T _2h = -2 / height;
			return{
				{ _2w, 0, 0, (T)-1 },
				{ 0, _2h, 0, 1 },
				{ 0, 0, (T)-1, 1 },
				{ 0, 0, 0, 1 }
			};
		}
		template <typename T, bool aligned>
		ATTR_INLINE const matrix<T, 4, 4, aligned> matrix_method<T, 4, 4, aligned>::orthogonal(T left, T top, T right, T bottom, T near, T far) noexcept
		{
			T _w = 1 / (left - right);
			T _h = 1 / (bottom - top);
			T _z = 1 / (near - far);

			return{
				{ (T)(-2 * _w), 0, 0, (T)((right + left)*_w) },
				{ 0, (T)(-2 * _h), 0, (T)((top + bottom)*_h) },
				{ 0, 0, _z, (T)(-far*_z) },
				{ 0, 0, 0, 1 }
			};
		}
		template <typename T, bool aligned>
		ATTR_INLINE const matrix<T, 4, 4, aligned> matrix_method<T, 4, 4, aligned>::fov(T fovy, T aspect, T _near, T _far) noexcept
		{
			T h = (T)(1.f / math::tan((float)fovy * 0.5f));
			T w = h / aspect;
			T Q = _far / (_near - _far);

			return{
				{ w, 0, 0, 0 },
				{ 0, h, 0, 0 },
				{ 0, 0, (T)Q, (T)(Q*_near) },
				{ 0, 0, (T)-1, 0 }
			};
		}

#include "mmx.h"

		namespace
		{
			inline void ignore_init(void * t) noexcept { };
			inline void m4x4_inverse(float * dest, const float * src) noexcept
			{
				__m128 minor0, minor1, minor2, minor3;
				__m128 row0, row1, row2, row3;
				__m128 det, tmp1;
				ignore_init(&tmp1);
				ignore_init(&row1);
				ignore_init(&row3);

				tmp1 = _mm_loadh_pi(_mm_loadl_pi(tmp1, (__m64*)(src)), (__m64*)(src + 4));
				row1 = _mm_loadh_pi(_mm_loadl_pi(row1, (__m64*)(src + 8)), (__m64*)(src + 12));
				row0 = _mm_shuffle_ps(tmp1, row1, 0x88);
				row1 = _mm_shuffle_ps(row1, tmp1, 0xDD);
				tmp1 = _mm_loadh_pi(_mm_loadl_pi(tmp1, (__m64*)(src + 2)), (__m64*)(src + 6));
				row3 = _mm_loadh_pi(_mm_loadl_pi(row3, (__m64*)(src + 10)), (__m64*)(src + 14));
				row2 = _mm_shuffle_ps(tmp1, row3, 0x88);
				row3 = _mm_shuffle_ps(row3, tmp1, 0xDD);
				// -----------------------------------------------
				tmp1 = _mm_mul_ps(row2, row3);
				tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
				minor0 = _mm_mul_ps(row1, tmp1);
				minor1 = _mm_mul_ps(row0, tmp1);
				tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
				minor0 = _mm_sub_ps(_mm_mul_ps(row1, tmp1), minor0);
				minor1 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor1);
				minor1 = _mm_shuffle_ps(minor1, minor1, 0x4E);
				// -----------------------------------------------
				tmp1 = _mm_mul_ps(row1, row2);
				tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
				minor0 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor0);
				minor3 = _mm_mul_ps(row0, tmp1);
				tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
				minor0 = _mm_sub_ps(minor0, _mm_mul_ps(row3, tmp1));
				minor3 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor3);
				minor3 = _mm_shuffle_ps(minor3, minor3, 0x4E);
				// -----------------------------------------------
				tmp1 = _mm_mul_ps(_mm_shuffle_ps(row1, row1, 0x4E), row3);
				tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
				row2 = _mm_shuffle_ps(row2, row2, 0x4E);
				minor0 = _mm_add_ps(_mm_mul_ps(row2, tmp1), minor0);
				minor2 = _mm_mul_ps(row0, tmp1);
				tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
				minor0 = _mm_sub_ps(minor0, _mm_mul_ps(row2, tmp1));
				minor2 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor2);
				minor2 = _mm_shuffle_ps(minor2, minor2, 0x4E);
				// -----------------------------------------------
				tmp1 = _mm_mul_ps(row0, row1); // Streaming SIMD Extensions - Inverse of 4x4 Matrix

				tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
				minor2 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor2);
				minor3 = _mm_sub_ps(_mm_mul_ps(row2, tmp1), minor3);
				tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
				minor2 = _mm_sub_ps(_mm_mul_ps(row3, tmp1), minor2);
				minor3 = _mm_sub_ps(minor3, _mm_mul_ps(row2, tmp1));
				// -----------------------------------------------
				tmp1 = _mm_mul_ps(row0, row3);
				tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
				minor1 = _mm_sub_ps(minor1, _mm_mul_ps(row2, tmp1));
				minor2 = _mm_add_ps(_mm_mul_ps(row1, tmp1), minor2);
				tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
				minor1 = _mm_add_ps(_mm_mul_ps(row2, tmp1), minor1);
				minor2 = _mm_sub_ps(minor2, _mm_mul_ps(row1, tmp1));
				// -----------------------------------------------
				tmp1 = _mm_mul_ps(row0, row2);
				tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
				minor1 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor1);
				minor3 = _mm_sub_ps(minor3, _mm_mul_ps(row1, tmp1));
				tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
				minor1 = _mm_sub_ps(minor1, _mm_mul_ps(row3, tmp1));
				minor3 = _mm_add_ps(_mm_mul_ps(row1, tmp1), minor3);
				// -----------------------------------------------
				det = _mm_mul_ps(row0, minor0);
				det = _mm_add_ps(_mm_shuffle_ps(det, det, 0x4E), det);
				det = _mm_add_ss(_mm_shuffle_ps(det, det, 0xB1), det);
				tmp1 = _mm_rcp_ss(det);
				det = _mm_sub_ss(_mm_add_ss(tmp1, tmp1), _mm_mul_ss(det, _mm_mul_ss(tmp1, tmp1)));
				det = _mm_shuffle_ps(det, det, 0x00);
				minor0 = _mm_mul_ps(det, minor0);
				_mm_storel_pi((__m64*)(dest), minor0);
				_mm_storeh_pi((__m64*)(dest + 2), minor0);
				minor1 = _mm_mul_ps(det, minor1);
				_mm_storel_pi((__m64*)(dest + 4), minor1);
				_mm_storeh_pi((__m64*)(dest + 6), minor1);
				minor2 = _mm_mul_ps(det, minor2);
				_mm_storel_pi((__m64*)(dest + 8), minor2);
				_mm_storeh_pi((__m64*)(dest + 10), minor2);
				minor3 = _mm_mul_ps(det, minor3);
				_mm_storel_pi((__m64*)(dest + 12), minor3);
				_mm_storeh_pi((__m64*)(dest + 14), minor3);
			}
		}
		template <>
		ATTR_INLINE const mat4a matrix_method<float, 4, 4, true>::inverse() const noexcept
		{
			mat4a out;
			m4x4_inverse(&out[0][0], &v[0][0]);
			return out;
		}
		template <>
		ATTR_INLINE void matrix_method<float, 4, 4, true>::scaling(float scale) noexcept
		{
			vec4a vscale = vec4a::makes(scale);
			vscale = vscale.shuffle<0, 0, 0, 1>() + CV_0001;
			preScale(vscale);
		}
		template <>
		ATTR_INLINE void matrix_method<float, 4, 4, true>::postScale(const vec4a & scale) noexcept
		{
			v[0] *= scale.getXV();
			v[1] *= scale.getYV();
			v[2] *= scale.getZV();
			v[3] *= scale.getWV();
		}
		template <>
		ATTR_INLINE const mat4a matrix_method<float, 4, 4, true>::world(const vec4a &pos, const vec4a &forward, const vec4a &up) noexcept
		{
			vec4a z = -forward.normalize();  // Forward
			vec4a x = cross(up, z).normalize(); // Right
			vec4a y = cross(z, x);

			return{
				{ x.x, y.x, z.x, pos.x },
				{ x.y, y.y, z.y, pos.y },
				{ x.z, y.z, z.z, pos.z },
				CV_0001
			};
		}
		template <>
		ATTR_INLINE const mat4a matrix_method<float, 4, 4, true>::look(const vec4a &eye, const vec4a &forward, const vec4a &up) noexcept
		{
			vec4a z = -forward.normalize();  // Forward
			vec4a x = cross(up, z).normalize(); // Right
			vec4a y = cross(z, x);
			x.w = (float)-dot(x, eye);
			y.w = (float)-dot(y, eye);
			z.w = (float)-dot(z, eye);
			return{ x,y,z, CV_0001 };
		}
		template <>
		ATTR_INLINE const mat4a matrix_method<float, 4, 4, true>::lookAt(const vec4a &eye, const vec4a &to, const vec4a &up) noexcept
		{
			return look(eye, to - eye, up);
		}
		template <>
		ATTR_INLINE const mat4a matrix_method<float, 4, 4, true>::translate(vector<float, 3> pos) noexcept
		{
			return{
				{ 1,0,0,pos.x },
				{ 0,1,0,pos.y },
				{ 0,0,1,pos.z },
				CV_0001
			};
		}
		template <>
		ATTR_INLINE const mat4a matrix_method<float, 4, 4, true>::identity() noexcept
		{
			return{
				CV_1000,
				CV_0100,
				CV_0010,
				CV_0001
			};
		}
		template <>
		ATTR_INLINE const mat4a matrix_method<float, 4, 4, true>::scale(const vec4a &scale) noexcept
		{
			return{
				scale & (vec4a&)CV_MASK_X,
				scale & (vec4a&)CV_MASK_Y,
				scale & (vec4a&)CV_MASK_Z,
				CV_0001
			};
		}
		template <>
		ATTR_INLINE const mat4a matrix_method<float, 4, 4, true>::scale(float x) noexcept
		{
			vec4a v(x);
			return scale(v);
		}
		template <>
		ATTR_INLINE const mat4a matrix_method<float, 4, 4, true>::rotateX(float rad) noexcept
		{
			float s, c;
			math::sincos(&s, &c, rad);

			vec4a vSin = vec4a::makes(s), vCos = vec4a::makes(c);
			vCos = shuffle<3, 0, 0, 3>(vCos, vSin);

			return{
				CV_1000,
				vCos * CV_MINUS_Z,			// { 0,c,-s,0 }
				vCos.shuffle<0, 2, 1, 3>(),	// { 0,s,c,0 }
				CV_0001
			};
		}
		template <>
		ATTR_INLINE const mat4a matrix_method<float, 4, 4, true>::rotateY(float rad) noexcept
		{
			float s, c;
			math::sincos(&s, &c, rad);

			vec4a vSin = vec4a::makes(s), vCos = vec4a::makes(c);
			vCos = shuffle<0, 3, 0, 3>(vCos, vSin);

			return{
				vCos,										// { c,0,s,0 }
				CV_0100,
				vCos.shuffle<2, 1, 0, 3>() * CV_MINUS_X,	// { -s,0,c,0 }
				CV_0001
			};
		}
		template <>
		ATTR_INLINE const mat4a matrix_method<float, 4, 4, true>::rotateZ(float rad) noexcept
		{
			float s, c;
			math::sincos(&s, &c, rad);
			vec4a cs = { c, s, 0, 0 };

			return{
				cs * CV_MINUS_Y,							// { c,-s,0,0 }
				cs.shuffle<1, 0, 2, 3>(),					// { s,c,0,0 }
				CV_0010,
				CV_0001
			};
		}
		template <>
		ATTR_INLINE const mat4a matrix_method<float, 4, 4, true>::orthogonal(float width, float height) noexcept
		{
			vec4a v = CV_P2N2_0_P1 / vec4a{ width , height , 1, 1 };
			return{
				shuffle<0,2,1,3>(v, CV_MINUS_W),	// w, 0, 0, -1
				v.shuffle<2,1,2,3>(),				// 0, h, 0, 1
				CV_00_N1_P1,
				CV_0001
			};
		}
		template <>
		ATTR_INLINE const mat4a matrix_method<float, 4, 4, true>::orthogonal(float left, float top, float right, float bottom, float _near, float _far) noexcept
		{
			vec4a v = {
				left - right,
				bottom - top,
				1.f,
				_near - _far
			};
			/*
			{ (T)(-2 * _w), 0, 0, (T)((right + left)*_w) },
			{ 0, (T)(-2 * _h), 0, (T)((top + bottom)*_h) },
			{ 0, 0, _z, (T)((_far + _near)*_z + (T)0.5) },
			{ 0, 0, 0, 1 }
			*/
			vec4a v2 = {
				left + right,
				top + bottom,
				0.f,
				-_far
			};

			v2 /= v;
			v = CV_N22_0_P1 / v;

			mat4a out;
			out[0] = v2.shuffle<2, 2, 2, 0>().adds(v);
			out[1] = shuffle<2, 1, 2, 1>(v, v2);
			out[2] = unpack_high(v, v2);
			out[3] = CV_0001;
			return out;
		}
		template <>
		ATTR_INLINE const mat4a matrix_method<float, 4, 4, true>::fov(float fovy, float aspect, float _near, float _far) noexcept
		{
			float h = 1.f / math::tan(fovy * 0.5f);
			float w = h / aspect;
			float Q = _far / (_near - _far);

			vec4a v = { w,h,Q, Q*_near };

			return{
				v & (vec4a&)CV_MASK_X,
				v & (vec4a&)CV_MASK_Y,
				v & (vec4a&)CV_MASK_ZW,
				CV_NEG_0010
			};
		}


		template <typename T, size_t cols, bool aligned>
		ATTR_INLINE const regionwh<T, cols> intersect(const regionwh<T, cols>& a, const regionwh<T, cols>& b) noexcept
		{
			matrix<T, 2, cols, aligned, matrix_data_type::width_height> out;
			for (size_t i = 0; i < cols; i++)
			{
				T av = a.pos[i], bv = b.pos[i];
				out.pos[i] = (av > bv) ? av : bv;
				av += a.size[i]; bv += b.size[i];
				out.size[i] = ((av < bv) ? av : bv) - out.pos[i];
			}
			return out;
		}
		template <typename T, size_t cols, bool aligned>
		ATTR_INLINE const region<T, cols> intersect(const region<T, cols>& a, const region<T, cols>& b) noexcept
		{
			matrix<T, 2, cols, aligned, matrix_data_type::right_bottom> out;
			for (size_t i = 0; i < cols; i++)
			{
				T av = a.from[i], bv = b.from[i];
				out.from[i] = (av > bv) ? av : bv;
				av = a.to[i]; bv = b.to[i];
				out.to[i] = (av < bv) ? av : bv;
			}
			return out;
		}
	}
}