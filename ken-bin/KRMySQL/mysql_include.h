#pragma once

#ifdef __MYSQL_REFERENCE_H
#error include before mysql_ref.h 
#endif

#define _CRT_NO_TIME_T

extern "C"
{
	_Check_return_wat_
		_ACRTIMP errno_t __cdecl _localtime32_s(
			_Out_ struct tm*        _Tm,
			_In_  __time32_t const* _Time
			);

	_Check_return_wat_
		_ACRTIMP errno_t __cdecl _localtime64_s(
			_Out_ struct tm*        _Tm,
			_In_  __time64_t const* _Time
			);

	_Check_return_wat_
		_ACRTIMP errno_t __cdecl _gmtime32_s(
			_Out_ struct tm*        _Tm,
			_In_  __time32_t const* _Time
			);

	_Check_return_wat_
		_ACRTIMP errno_t __cdecl _gmtime64_s(
			_Out_ struct tm*        _Tm,
			_In_  __time64_t const* _Time
			);

#ifdef _USE_32BIT_TIME_T

#if __STDC_WANT_SECURE_LIB__

	_Check_return_wat_
		static __inline errno_t __CRTDECL gmtime_s(
			_Out_ struct tm*    const _Tm,
			_In_  time_t const* const _Time
			)
	{
		return _gmtime32_s(_Tm, _Time);
	}

	_Check_return_wat_
		static __inline errno_t __CRTDECL localtime_s(
			_Out_ struct tm*    const _Tm,
			_In_  time_t const* const _Time
			)
	{
		return _localtime32_s(_Tm, _Time);
	}
#endif

#else // ^^^ _USE_32BIT_TIME_T ^^^ // vvv !_USE_32BIT_TIME_T vvv

#if __STDC_WANT_SECURE_LIB__

	_Check_return_wat_
		static __inline errno_t __CRTDECL gmtime_s(
			_Out_ struct tm*    const _Tm,
			_In_  time_t const* const _Time
			)
	{
		return _gmtime64_s(_Tm, _Time);
	}

	_Check_return_wat_
		static __inline errno_t __CRTDECL localtime_s(
			_Out_ struct tm*    const _Tm,
			_In_  time_t const* const _Time
			)
	{
		return _localtime64_s(_Tm, _Time);
	}

#endif

#endif // !_USE_32BIT_TIME_T


}

#include <math.h>
#define HAVE_RINT
#include <KR3/wl/prepare.h>
#pragma warning(disable:4996)
#pragma warning(push)
#undef _WIN32_WINNT
#include <my_global.h>
#include <mysql.h>
#pragma warning(pop)
#include <KR3/wl/clean.h>
