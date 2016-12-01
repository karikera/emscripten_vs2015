#pragma once

#include "text.h"

namespace kr
{
	enum class ConsoleType
	{
		Standard,
		Error,
		Debug
	};
	template <ConsoleType type, typename C>
	class ConsoleOutputStream;
	template <typename C>
	using StandardOutputT = ConsoleOutputStream<ConsoleType::Standard, C>;
	template <typename C>
	using StandardErrorOutputT = ConsoleOutputStream<ConsoleType::Error, C>;
	template <typename C>
	using DebugOutputT = ConsoleOutputStream<ConsoleType::Debug, C>;
	using StandardOutput = StandardOutputT<char>;
	using StandardOutput16 = StandardOutputT<char16>;
	using StandardOutputW = StandardOutputT<wchar>;
	using StandardErrorOutput = StandardErrorOutputT<char>;
	using StandardErrorOutput16 = StandardErrorOutputT<char16>;
	using StandardErrorOutputW = StandardErrorOutputT<wchar>;
	using DebugOutput = DebugOutputT<char>;
	using DebugOutput16 = DebugOutputT<char16>;
	using DebugOutputW = DebugOutputT<wchar>;

	template <ConsoleType type, typename C>
	class ConsoleOutputStream: public OutStream<ConsoleOutputStream<type, C>, C>
	{
	public:
		void flush() noexcept;
		void writeImpl(const C *chr, size_t sz) noexcept;

		static ConsoleOutputStream out;
	};

	template <typename C>
	class ConsoleOutputStream<ConsoleType::Debug, C> :
		public OutStream<ConsoleOutputStream<ConsoleType::Debug, C>, C>
	{
	public:
		void writeImpl(const C *chr, size_t sz) noexcept;
		void putSourceLine(const C * src, int line) noexcept;
		void flush() noexcept;

		static ConsoleOutputStream out;
	};
#ifdef WIN32
	template <>
	class ConsoleOutputStream<ConsoleType::Debug, wchar> :
		public OutStream<ConsoleOutputStream<ConsoleType::Debug, wchar>, wchar>
	{
	public:
		ConsoleOutputStream() noexcept;
		~ConsoleOutputStream() noexcept;
		void writeImpl(const wchar * chr, size_t sz) noexcept;
		void putSourceLine(pcwstr src, int line) noexcept;
		void flush() noexcept;

		static ConsoleOutputStream<ConsoleType::Debug, wchar> out;

	private:
#ifndef NDEBUG
		int _thread() noexcept;
#endif
	};
	
#else
	template <>
	void DebugOutput::putSourceLine(pcstr src, int line) noexcept;
	template <>
	void DebugOutput::flush() noexcept;
#endif

	extern StandardOutput &cout;
	extern StandardOutput16 &ucout;
	extern StandardOutputW &wcout;
	extern StandardErrorOutput &cerr;
	extern StandardErrorOutput16 &ucerr;
	extern StandardErrorOutputW &wcerr;
	extern DebugOutput &dout;
	extern DebugOutput16 &udout;
	extern DebugOutputW &wdout;

	template <ConsoleType type, typename C>
	ConsoleOutputStream<type,C> ConsoleOutputStream<type, C>::out;

	template <typename C>
	ConsoleOutputStream<ConsoleType::Debug, C> ConsoleOutputStream<ConsoleType::Debug, C>::out;

	template <>
	void DebugOutput::writeImpl(const char *chr, size_t sz) noexcept;
	template <>
	void StandardOutput::flush() noexcept;
	template <>
	void StandardOutput::writeImpl(const char * chr, size_t sz) noexcept;
	template <>
	void StandardOutputW::flush() noexcept;
	template <>
	void StandardOutputW::writeImpl(const wchar * chr, size_t sz) noexcept;
	template <>
	void StandardErrorOutput::flush() noexcept;
	template <>
	void StandardErrorOutput::writeImpl(const char * chr, size_t sz) noexcept;
	template <>
	void StandardErrorOutputW::flush() noexcept;
	template <>
	void StandardErrorOutputW::writeImpl(const wchar * chr, size_t sz) noexcept;

	template <ConsoleType type, typename C>
	inline void ConsoleOutputStream<type, C>::flush() noexcept
	{
		ConsoleOutputStream<type, char>::flush();
	}
	template <ConsoleType type, typename C>
	inline void ConsoleOutputStream<type, C>::writeImpl(const C *chr, size_t sz) noexcept
	{
		ConsoleOutputStream<type, char>::out << (ToAcp<C>)RefArray<C>(chr, sz);
	}
	template <typename C>
	inline void ConsoleOutputStream<ConsoleType::Debug, C>::writeImpl(const C *chr, size_t sz) noexcept
	{
#ifdef WIN32
		if (sizeof(C) == sizeof(wchar))
			wdout << TextW((const wchar*)chr, sz);
		else
			dout << toAcp(RefArray<C>(chr, sz));
#else
		dout << toAcp(RefArray<C>(chr, sz));
#endif
	}
	template <typename C>
	inline void ConsoleOutputStream<ConsoleType::Debug, C>::putSourceLine(const C * src, int line) noexcept
	{
		*this << src << (C)'(' << line << (C)')' << (C)'\r' << (C)'\n';
	}
	template <typename C>
	inline void ConsoleOutputStream<ConsoleType::Debug, C>::flush() noexcept
	{
#ifdef WIN32
		wdout.flush();
#else
		dout.flush();
#endif
	}

}