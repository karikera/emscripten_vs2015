#pragma once

#include <KR3/main.h>
#include <KR3/io/bufferedstream.h>

namespace kr
{
	class Parser
	{
	public:
		Parser(io::VIStream<char> is) noexcept;

		void skipWhiteSpace();
		int getLine() noexcept;

	protected:
		io::BufferedIStream<io::VIStream<char>, true> m_is;
		int m_line;
	};
}