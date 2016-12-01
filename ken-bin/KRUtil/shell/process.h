#pragma once

#ifndef WIN32
#error is not windows system
#endif

#include <KR3/main.h>
#include <KR3/wl/windows.h>

namespace kr
{
	class Process
	{
	public:
		enum Shell_t { Shell };
		enum ShellMove_t { ShellMove };
		Process() noexcept;
		Process(Shell_t, TextW command) noexcept;
		Process(pcwstr fileName, pwstr parameter) noexcept;
		~Process() noexcept;

		void close() noexcept;
		void shell(TextW command, pcwstr curdir = nullptr) noexcept;
		void exec(pcwstr fileName, pwstr parameter, pcwstr curdir = nullptr) noexcept;
		void skipTo(char chr) noexcept;
		void printTo(char chr) noexcept;
		TSZ readTo(char chr) noexcept;
		void passThrough() noexcept;
		bool eof() noexcept;

		DWORD getExitCode() noexcept;

	private:
		bool _read() noexcept;

		HANDLE m_process;
		HANDLE m_stdout_read;
		char m_buffer[4096];
		char * m_readed;
		bool m_eof;

	};
}
