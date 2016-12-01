#pragma once

#include <KR3/main.h>
#include <KR3/io/selfbufferedstream.h>

namespace kr
{
	
	template <typename IS>
	class CSVReader
	{
	public:
		static_assert(IsIStream<IS>::value, "IS is not InStream");
		using Component = typename IS::Component;

		CSVReader(nullptr_t);
		CSVReader(IS* stream);
		CSVReader(const CSVReader&)=delete;
		CSVReader<IS>& operator = (nullptr_t);
		CSVReader<IS>& operator = (IS* stream);
		IS* operator &() const;

		void skip(); // EofException, NoLineException
		template <class Derived, typename C, typename _Info>
		void next(OutStream<Derived, C, _Info>* dest); // EofException, NoLineException
		TmpArray<Component> next(); // EofException, NoLineException
		Array<Component> nextAlloc(); // EofException, NoLineException
		size_t nextLength();
		void mustLine(); // EofException, NoLineException
		bool nextLine();
		int getLine() noexcept;

	private:
		io::SelfBufferedIStream<IS> m_stream;
		int m_nLast;
		int m_line;

	};

	template <typename OS>
	class CSVWriter
	{
	private:
		static_assert(IsOStream<OS>::value, "OS is not OutStream");

	public:
		using Component = typename OS::Component;

		CSVWriter(nullptr_t) noexcept;
		CSVWriter(OS* stream) noexcept;
		CSVWriter(const CSVWriter&)=delete;
		CSVWriter& operator = (nullptr_t) noexcept;
		CSVWriter& operator = (OS* stream) noexcept;
		OS* operator &() const noexcept;
		
		template <typename Converter, typename C2>
		void write(RefArray<C2> str); // NotEnoughSpaceException
		void write(RefArray<Component> str); // NotEnoughSpaceException
		void nextLine(); // NotEnoughSpaceException

	private:
		OS* m_stream;
		bool m_prev;
	};

	class ParsingException
	{
	};
	class NextLineException:public ParsingException
	{
	};
	class NoLineException:public ParsingException
	{
	};

}

#include "csvreader.inl"