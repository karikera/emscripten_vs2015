#pragma once

#include <KR3/main.h>

namespace kr
{
#ifdef WIN32
	void execOpen(pcwstr path) noexcept;
#else
	void execOpen(pcstr path) noexcept;
#endif
}
