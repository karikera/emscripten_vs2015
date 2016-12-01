#pragma once

#include "../../main.h"
#include "../../meta/number.h"

#include <utility>
#include "ref.h"

namespace kr
{
	template <typename T, size_t size> class TFixedBase
	{
	public:
		int_sz<size> fract;
		T value;
	};

	template <typename T, size_t size>
	class TFixed:public TFixedBase<T, size>
	{
		using Super = TFixedBase<T, size>;
	public:
		using TU = int_sz<size>;
		using Super::fract;
		using Super::value;

		TFixed()
		{
		}
		TFixed(T n)
		{
			value = n;
			fract = 0;
		}
		TFixed(T v, TU v2)
		{
			value=v;
			fract=v2;
		}
		explicit TFixed(float n)
		{
			value = (T)math::floor(n);
			fract = (TU)((n - value) * ((1 << (sizeof(fract)* 8)) -1));
		}
		explicit TFixed(double n)
		{
			value = (T)floorf(n);
			fract = decltype(fract)((n - value) * ((1 << (sizeof(fract)* 8)) -1));
		}
		template <typename T3, intp size2>
		explicit TFixed(const TFixed<T3, size2> &o)
		{
			const intp shift=(size - sizeof(o.fract));
			value = (T)o.value;
			fract=(TU)((shift >= 0) ? (o.fract << shift) : (o.fract >> -shift));
		}
		explicit operator int() const
		{
			return (int)value;
		}
		explicit operator float() const
		{
			return (float)(raw(*this))/overof(TU);
		}
		explicit operator double() const
		{
			return (double)(raw(*this))/overof(TU);
		}
		
		const TFixed operator +(const TFixed &_v) const
		{
			if(sizeof(TFixed) == 8 || sizeof(TFixed) == 4 || sizeof(TFixed) == 2 || sizeof(TFixed) == 1)
			{
				auto _out = raw(*this) + raw(_v);
				return move((TFixed&)_out);
			}

			TFixed _out;
			_out.value = math::addc<T>(&_out.fract, fract, _v.fract) + value + _v.value;
			return _out;
		}
		const TFixed operator -(const TFixed &_v) const
		{
			if(sizeof(TFixed) == 8 || sizeof(TFixed) == 4 || sizeof(TFixed) == 2 || sizeof(TFixed) == 1)
			{
				auto _out = raw(*this) + raw(_v);
				return move((TFixed&)_out);
			}

			TFixed _out;
			_out.value = math::subb<T>(&_out.fract, fract, _v.fract) + value - _v.value;
			return _out;
		}
		const TFixed operator *(const TFixed &o) const
		{
			auto _out = (raw(*this) * raw(o))>>(size*8);
			return (TFixed&)_out;
		}
		const TFixed operator /(const TFixed &o) const
		{
			auto _out = (raw(*this) << (size * 8)) / raw(o);
			return (TFixed&)_out;
		}
		const TFixed operator +(intp n) const
		{
			return TFixed(value + n, fract);
		}
		const TFixed operator -(intp n) const
		{
			return TFixed(value - n, fract);
		}
		friend TFixed&& operator +(intp n, const TFixed &o)
		{
			return o + n;
		}
		friend TFixed&& operator -(intp n, const TFixed &o)
		{
			return TFixed(n - value, fract);
		}
		const TFixed operator -() const
		{
			if(sizeof(TFixed) == 8 || sizeof(TFixed) == 4 || sizeof(TFixed) == 2 || sizeof(TFixed) == 1)
			{
				return -raw(*this);
			}
			TFixed out;
			out.fract = -fract;
			math::addc<T>(&out.value,value, 0);
			out.value = -value;
			return out;
		}
		TFixed& operator +=(const TFixed &o)
		{
			return *this = *this + o;
		}
		TFixed& operator -=(const TFixed &o)
		{
			return *this = *this - o;
		}
		TFixed& operator *=(const TFixed &o)
		{
			return *this = *this * o;
		}
		TFixed& operator /=(const TFixed &o)
		{
			return *this = *this / o;
		}
		TFixed& operator +=(intp n)
		{
			value += n;
			return *this;
		}
		TFixed& operator -=(intp n)
		{
			value -= n;
			return *this;
		}
		TFixed& operator ++()
		{
			value++;
			return *this;
		}
		TFixed& operator --()
		{
			value--;
			return *this;
		}
	};

	template <typename T, size_t size>
	class TFixedPoint:public TFixedBase<T*, size>
	{
		using Super = TFixedBase<T*, size>;
	public:
		using Super::value;
		using Super::fract;
		using TU = int_sz<size>;

		TFixedPoint()
		{
		}
		TFixedPoint(T* n)
		{
			value = n;
			fract = 0;
		}
		TFixedPoint(T* v, TU v2)
		{
			value=v;
			fract=v2;
		}
		template <typename T3, intp size2> explicit TFixedPoint(const TFixed<T3, size2> &o)
		{
			const intp shift=(size - sizeof(o.fract));
			value = (T*)o.value;
			fract=(TU)((shift >= 0) ? (o.fract << shift) : (o.fract >> -shift));
		}
		template <typename T3, intp size2> const TFixedPoint operator +(const TFixed<T3, size2> &_v) const
		{
			TFixedPoint _out;
			_out.value = math::addc<T3>(&_out.fract, fract, _v.fract) + value + _v.value;
			return _out;
		}
		template <typename T3, intp size2> const TFixedPoint operator -(const TFixed<T3, size2> &_v) const
		{
			TFixedPoint _out;
			_out.value = math::subb<T3>(&_out.fract, fract, _v.fract) + value - _v.value;
			return _out;
		}
		const TFixedPoint operator +(intp n) const
		{
			return TFixedPoint(value + n, fract);
		}
		const TFixedPoint operator -(intp n) const
		{
			return TFixedPoint(value - n, fract);
		}
		template <typename T3, intp size2> TFixedPoint& operator +=(const TFixed<T3, size2> &o)
		{
			return *this = *this + o;
		}
		template <typename T3, intp size2> TFixedPoint& operator -=(const TFixed<T3, size2> &o)
		{
			return *this = *this - o;
		}
		TFixedPoint& operator +=(intp n)
		{
			value += n;
			return *this;
		}
		TFixedPoint& operator -=(int n)
		{
			value -= n;
			return *this;
		}
		TFixedPoint& operator ++()
		{
			value++;
			return *this;
		}
		TFixedPoint& operator --()
		{
			value--;
			return *this;
		}
		T& operator *()
		{
			return *value;
		}
		T& operator [](intp i)
		{
			return value[i];
		}
		T operator ->()
		{
			return value;
		}
	};

}