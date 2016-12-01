#pragma once

#include <errno.h>
#include <stdlib.h>
#include <emscripten.h>

#ifdef _MSC_VER

#include <sal.h>

#define ATTR_NULL_TERMINATED	__nullterminated
#define ATTR_CHECK_RETURN	_Check_return_
#define ATTR_NULLABLE		_Ret_maybenull_
#define ATTR_NONULL			_Ret_notnull_
#define ATTR_INLINE			__forceinline
#define ATTR_NOALIAS		__declspec(noalias)
#define ATTR_NORETURN		__declspec(noreturn)
#define ATTR_IMPORT			__declspec(dllimport)
#define ATTR_DEPRECATED		__declspec(deprecated)
#define ATTR_ANY			__declspec(selectany)
#define CT_STDCALL			__stdcall
#define CT_CDECL			__cdecl
#define CT_FASTCALL			__fastcall
#define CT_VECTORCALL		__vectorcall
#define debug_force()		__debugbreak()

#ifndef NDEBUG
#define ondebug(code) code
#define debug()	debug_force()
#define _assert(x)	do {if(!(x)) {\
	if(::kr::checkDebugging())\
		__debugbreak();\
} } while(0,0)
#else
#define ondebug(code)
#define debug()	do {} while(0,0)
#define _assert(x)	do {__assume(x); } while(0,0)
#endif

#elif defined(__GNUG__)

#include <signal.h>

#define ATTR_NULL_TERMINATED	
#define ATTR_CHECK_RETURN	__attribute__((warn_unused_result))
#define ATTR_NULLABLE		
#define ATTR_NONULL			
#define ATTR_INLINE			__attribute__((always_inline)) inline
#define ATTR_NOALIAS		
#define ATTR_NORETURN		[[noreturn]]
#define ATTR_IMPORT			
#define ATTR_DEPRECATED		[[deprecated]]
#define ATTR_ANY			__attribute__((weak))
#define CT_STDCALL			
#define CT_CDECL			
#define CT_FASTCALL			
#define CT_VECTORCALL		

#ifdef __EMSCRIPTEN__
#define debug_force() EM_ASM(debugger;)
#else
#define debug_force() raise(SIGTRAP)
#endif

#ifndef NDEBUG
#define ondebug(code) code
#define debug() debug_force()
#define _assert(x)	do { if(!(x)) { raise(SIGTRAP); } } while(0)
#else
#define ondebug(code)
#define debug()	do {} while(0)
#define _assert(x)	do { if (!(x)) __builtin_unreachable(); } while (0)
#endif

#else
#error Unknown compiler
#endif

#ifdef __EMSCRIPTEN__
#define NO_USE_FILESYSTEM
#define NO_USE_SOCKET
#endif


#ifdef NO_USE_SOCKET
#define NEED_SOCKET static_assert(false, "NO_USE_SOCKET");
#else
#define NEED_SOCKET
#endif
#ifdef NO_USE_FILESYSTEM
#define NEED_FILESYSTEM static_assert(false, "NO_USE_FILESYSTEM");
#else
#define NEED_FILESYSTEM
#endif

namespace kr
{
	bool CT_FASTCALL checkDebugging() noexcept;
	ATTR_NORETURN ATTR_INLINE void noreturn() noexcept
	{
		abort();
	}
	ATTR_NORETURN ATTR_INLINE void unreachable()
	{
		_assert(!"unreachable code");
		noreturn();
	}
	ATTR_NORETURN void CT_FASTCALL terminate(int err) noexcept;
	ATTR_NORETURN void CT_FASTCALL notEnoughMemory() noexcept;

	unsigned long long CT_FASTCALL rdtsc() noexcept;
}

#define EMPTY_SOURCE namespace { char __kr_pri_dummy_for_LNK4221; }