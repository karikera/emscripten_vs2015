#pragma once

#include "compiler.h"

#include <type_traits>
#include <ostream>

namespace kr
{
	struct Undefined;

	// kr type
#define ptr_def(type) \
		using p##type = type*;\
		using c##type = const type;\
		using pc##type = const type*;

#define ptr_typedef(type, def) \
		using type = def;\
		ptr_def(type)

	ptr_typedef(intp, intptr_t);
	ptr_typedef(uintp, uintptr_t);
	ptr_typedef(llong, long long);
	ptr_typedef(uint, unsigned int);
	ptr_typedef(qword, unsigned long long);
	ptr_typedef(dword, unsigned long);
	ptr_typedef(word, unsigned short);
	ptr_typedef(byte, unsigned char);
	using filesize_t = qword;
	ptr_def(int);
	ptr_def(short);
	ptr_def(long);

	using ptr = void *;
	using cptr = const void *;

	using pstr = ATTR_NULL_TERMINATED char*;
	using pcstr = ATTR_NULL_TERMINATED const char*;

	using char16 = char16_t;
	using pstr16 = ATTR_NULL_TERMINATED char16*;
	using pcstr16 = ATTR_NULL_TERMINATED const char16*;
	
	using char32 = char32_t;
	using pstr32 = ATTR_NULL_TERMINATED char32*;
	using pcstr32 = ATTR_NULL_TERMINATED const char32*;

	using wchar = wchar_t;
	using pwstr = ATTR_NULL_TERMINATED wchar*;
	using pcwstr = ATTR_NULL_TERMINATED const wchar*;

	template <size_t sz>
	struct size_to_unicode;

	template <>
	struct size_to_unicode<1> { using type = char; };
	template <>
	struct size_to_unicode<2> { using type = char16; };
	template <>
	struct size_to_unicode<4> { using type = char32; };

	template <size_t sz>
	using size_to_unicode_t = typename size_to_unicode<sz>::type;

	using unicode_wchar = size_to_unicode_t<sizeof(wchar)>;

	#undef ptr_def
	#undef ptr_typedef
	
	constexpr int eof = -1;
		
	class nullref_t final
	{
	public:
		nullref_t() = delete;
		~nullref_t() = delete;
		nullref_t(const nullref_t&) = delete;
		nullref_t& operator =(const nullref_t&) = delete;

		template <typename T> bool operator ==(const T &t)
		{
			return &t == nullptr;
		}
		template <typename T> bool operator !=(const T &t)
		{
			return &t != nullptr;
		}
		template <typename T> operator T&()
		{
#pragma warning(push)
#pragma warning(disable:6011)
			return *(T*)nullptr;
#pragma warning(pop)
		}
		template <typename T> friend bool operator ==(const T &t, nullref_t&)
		{
			return &t == nullptr;
		}
		template <typename T> friend bool operator !=(const T &t, nullref_t&)
		{
			return &t != nullptr;
		}
	};
	
	static nullref_t &nullref = *(nullref_t*)nullptr;

	using std::endl;


	struct Range
	{
		size_t beg, end;
	};
}

using std::nullptr_t;
using kr::char16;
using kr::char32;
using kr::wchar;

namespace kr
{
	namespace type
	{
		using ::kr::intp;	using ::kr::pintp;	using ::kr::pcintp;		using ::kr::cintp;
		using ::kr::uintp;	using ::kr::puintp;	using ::kr::pcuintp;	using ::kr::cuintp;
		using ::kr::llong;	using ::kr::pllong;	using ::kr::pcllong;	using ::kr::cllong;
		using ::kr::uint;	using ::kr::puint;	using ::kr::pcuint;		using ::kr::cuint;
		using ::kr::qword;	using ::kr::pqword;	using ::kr::pcqword;	using ::kr::cqword;
		using ::kr::dword;	using ::kr::pdword;	using ::kr::pcdword;	using ::kr::cdword;
		using ::kr::word;	using ::kr::pword;	using ::kr::pcword;		using ::kr::cword;
		using ::kr::byte;	using ::kr::pbyte;	using ::kr::pcbyte;		using ::kr::cbyte;
		using ::kr::pint;	using ::kr::pcint;	using ::kr::cint;
		using ::kr::pshort;	using ::kr::pcshort;using ::kr::cshort;
		using ::kr::plong;	using ::kr::pclong;	using ::kr::clong;
		using ::kr::ptr;	using ::kr::cptr;
		using ::kr::pstr;	using ::kr::pcstr;
		using ::kr::pstr16;	using ::kr::pcstr16;
		using ::kr::pstr32;	using ::kr::pcstr32;
		using ::kr::pwstr;	using ::kr::pcwstr;
		using ::kr::filesize_t;
	}
}