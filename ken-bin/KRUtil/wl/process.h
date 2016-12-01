#pragma once

#ifndef WIN32
#error is not windows system
#endif

#include <KR3/main.h>

namespace kr
{
	class Process
	{
	public:
		// 0을 반환하면 성공.
		static int execute(pwstr pszstr) noexcept;

		// 0을 반환하면 성공.
		static int detachedExecute(pwstr pszstr) noexcept;

	};
}