#pragma once

#include "../main.h"

namespace kr
{
	struct ThrowAbort {};

	struct Exception {};
	struct DuplicateException {};
	struct NotFoundException:Exception {};
	struct NotEnoughSpaceException:Exception {};
	struct EofException:Exception {};
	struct OutOfRangeException:Exception {};
	struct InvalidSourceException:Exception {};
	struct TooBigException:Exception {};

	struct FileException:Exception {};
	struct QuitException:Exception
	{
		int exitCode;
		inline QuitException(int code) noexcept
		{
			exitCode = code;
		}
	};
	struct UnsupportedException:Exception {};

	struct DisconnectException:Exception {};

	ATTR_NORETURN void quit(int exitCode); // QuitException
	ATTR_NORETURN void error(const char * strMessage, ...) noexcept;
	void warning(const char * strMessage, ...) noexcept;
	template <typename T> bool intactTest(size_t v) noexcept;
	template <typename T> T intact(size_t v) noexcept
	{
		_assert(intactTest<T>(v));
		return (T)v;
	}
}

#define shouldbe(cmp)	((cmp) ? true : (::kr::warning(#cmp), false))
#define mustbe(cmp)		{ if(!(cmp)) ::kr::error(#cmp); }