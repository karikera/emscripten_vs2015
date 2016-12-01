#pragma once

#include "../main.h"
#include "../meta/boolidx.h"

namespace kr
{
	namespace math
	{

		constexpr float pi = 3.141592653589f;

		#define _RETURN_	ATTR_CHECK_RETURN ATTR_NOALIAS

		template <typename T> _RETURN_ T pow(T base, T exp) noexcept;
		template <> _RETURN_ float pow<float>(float x, float y) noexcept;
		template <> _RETURN_ double pow<double>(double x, double y) noexcept;

		template <typename T> _RETURN_ T powi(T base, int exp) noexcept;
		template <typename T> _RETURN_ T sqrt(T x) noexcept;
		template <typename T> _RETURN_ T abs(T a) noexcept;
		template <> _RETURN_ float abs<float>(float a) noexcept;
		template <> _RETURN_ double abs<double>(double a) noexcept;
		template <typename T, typename ... ARGS>
		_RETURN_ T max(T arg, ARGS ... args) noexcept;
		template <typename T, typename ... ARGS>
		_RETURN_ T min(T arg, ARGS ... args) noexcept;
		template <typename T>
		_RETURN_ bool between(T min, T value, T max) noexcept;
		template <typename T> _RETURN_ T getsign(T a) noexcept;
		template <typename T> _RETURN_ T getsigni(T a) noexcept;
		template <typename T2, typename T> _RETURN_ T2 addc(T* _res, T _a, T _b) noexcept;
		template <typename T2, typename T> _RETURN_ T2 subb(T* _res, T _a, T _b) noexcept;
		template <typename T> _RETURN_ int suspectedFloor(T value) noexcept;

		_RETURN_ dword pow2ceil(dword number) noexcept;
		_RETURN_ dword pow2floor(dword number) noexcept;
		template <typename T> _RETURN_ dword cipher(T n_number, uint n_radix) noexcept;
		_RETURN_ float sin(float a) noexcept;
		_RETURN_ float cos(float a) noexcept;
		void sincos(float *s, float *c, float a) noexcept;
		_RETURN_ float tan(float a) noexcept;
		_RETURN_ float asin(float a) noexcept;
		_RETURN_ float acos(float a) noexcept;
		_RETURN_ float atan(float a) noexcept;
		_RETURN_ float atan2(float y, float x) noexcept;
		_RETURN_ long gap(long a, long b) noexcept;
		_RETURN_ float ceil(float a) noexcept;
		_RETURN_ long ceil(long a, long b) noexcept;
		_RETURN_ uint ceil(uint a, uint b) noexcept;
		_RETURN_ float ceil(float a, float b) noexcept;
		_RETURN_ float round(float x) noexcept;
		_RETURN_ long lround(float x) noexcept;
		template <typename T> _RETURN_ T tround(float x) noexcept;
		template <> _RETURN_ float tround<float>(float x) noexcept;
		template <> _RETURN_ double tround<double>(float x) noexcept;
		template <> _RETURN_ unsigned long long tround<unsigned long long>(float x) noexcept;
		template <> _RETURN_ long long tround<long long>(float x) noexcept;
		_RETURN_ float floor(float a) noexcept;
		template <typename T>
		_RETURN_ T mod(T a, T b) noexcept;
		template <>
		_RETURN_ float mod<float>(float a, float b) noexcept;
		template <>
		_RETURN_ double mod<double>(double a, double b) noexcept;
		_RETURN_ float radianmod(float a) noexcept;
		_RETURN_ dword ilog(dword val, dword n) noexcept;
		_RETURN_ dword ilog2(dword val) noexcept;
		_RETURN_ dword plog2(uintp val) noexcept;

		template <typename T, typename ... ARGS> 
		_RETURN_ T sum(T arg, ARGS ... args) noexcept
		{
			T out = arg;
			unpackR(out += (T)args);
			return out;
		}

		template <typename T>
		_RETURN_ T clamp(T _min, T _v, T _max) noexcept
		{
			if (_v < _min)
				return _min;
			if (_v > _max)
				return _max;
			return _v;
		}

		#undef _RETURN_

	}

}

#include "math.inl"
