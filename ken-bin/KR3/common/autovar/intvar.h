#pragma once

#ifndef __KR3_INCLUDED
#error Need To Include KR3/main.h
#endif

namespace kr
{
	// int by size
	namespace _pri_
	{
		template <size_t size> struct __INTFROMSIZE{};
		template <> struct __INTFROMSIZE<1>{ using stype = char;	using utype = byte;		using ctype = char; };
		template <> struct __INTFROMSIZE<2>{ using stype = short;	using utype = word;		using ctype = wchar; };
		template <> struct __INTFROMSIZE<4>{ using stype = long;	using utype = dword;	using ctype = char32; };
		template <> struct __INTFROMSIZE<8>{ using stype = llong;	using utype = qword;	using ctype = qword;};
	}

	template <size_t size> using int_sz = typename _pri_::__INTFROMSIZE<size>::stype;
	template <size_t size> using uint_sz = typename _pri_::__INTFROMSIZE<size>::utype;
	template <size_t size> using char_sz = typename _pri_::__INTFROMSIZE<size>::ctype;

	// raw
	template <typename T> int_sz<sizeof(T)>& raw(T &data)
	{
		return (int_sz<sizeof(T)>&) data;
	}
}
