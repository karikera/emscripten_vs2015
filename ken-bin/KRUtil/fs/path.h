#pragma once

#include <KR3/main.h>

namespace kr
{
	class Path
	{
	public:
		constexpr static size_t MAX_LEN = 512;

		Path() noexcept;
		TextW get(TextW filename); // NotEnoughSpaceException
		pcwstr getsz(TextW filename); // NotEnoughSpaceException
		wchar * enter(TextW name); // NotEnoughSpaceException
		void leave(wchar * path) noexcept;
		pcwstr getCurrentDirectorySz() noexcept;

	private:
		wchar * m_path;
		wchar m_buffer[MAX_LEN];
	};
}
