#pragma once

#include <KR3/meta/text.h>

template <typename IS> 
kr::CSVReader<IS>::CSVReader(nullptr_t):m_stream(nullptr_t)
{
	m_nLast = ',';
	m_line = 1;
}
template <typename IS> 
kr::CSVReader<IS>::CSVReader(IS* stream):m_stream(stream)
{
	m_nLast = ',';
	m_line = 1;
	m_stream.need(3);
	if (m_stream.text() == "\xEF\xBB\xBF")
	{
		m_stream.skip(3);
	}
}
template <typename IS> 
kr::CSVReader<IS>& kr::CSVReader<IS>::operator = (nullptr_t)
{
	m_stream = nullptr;
	return *this;
}
template <typename IS> 
kr::CSVReader<IS>& kr::CSVReader<IS>::operator = (IS* stream)
{
	m_stream = stream;
	return *this;
}
template <typename IS> 
IS* kr::CSVReader<IS>::operator &() const
{
	return &m_stream;
}

template <typename IS> 
size_t kr::CSVReader<IS>::nextLength()
{
	size_t len = 0;
	try
	{
		char chr = m_stream.peek();
		size_t start = 0;
		if(chr == '\"')
		{
			start++;
			size_t pos = start;
			for(;;)
			{
				try
				{
					pos = m_stream.pos('\"', pos);
					if(m_stream.peek(pos+1) != '\"')
					{
						len += pos - start;
						start = pos+1;
						break;
					}
					else
					{
						pos++;
						len += pos - start;
						pos++;
						start = pos;
					}
				}
				catch(EofException&)
				{
					return len;
				}
			}
		}

		size_t pos = m_stream.pos_y(",\n", start);
		return pos-start + len;
	}
	catch(EofException&)
	{
		return eof;
	}
}
template <typename IS> 
void kr::CSVReader<IS>::mustLine()
{
	switch(m_nLast)
	{
	case '\n': m_nLast = ','; break;
	case eof: throw EofException();
	default: throw NoLineException();
	}
	m_line++;
}
template <typename IS> 
bool kr::CSVReader<IS>::nextLine()
{
	if(m_nLast == '\n')
	{
		m_nLast = ',';
		m_line++;
		return true;
	}
	try
	{
		char finded;
		do
		{
			char chr = m_stream.peek();
			if(chr == '\"')
			{
				m_stream.skip(1);

				size_t pos = 0;
				for(;;)
				{
					try
					{
						pos = m_stream.pos('\"', pos);
						pos++;
						if(m_stream.peek(pos) != '\"')
						{
							m_stream.skip(pos);
							break;
						}
						else
						{
							m_stream.skip(pos + 1);
							pos = 0;
						}
					}
					catch(TooBigException&)
					{
						m_stream.clearBuffer();
					}
				}
			}

			m_stream.readto_y(",\n", &finded);
		}
		while(finded == ',');
		m_nLast = ',';
		m_line++;
		return true;
	}
	catch(EofException&)
	{
		m_stream.clearBuffer();
		m_nLast = eof;
		return false;
	}
}
template <typename IS>
int kr::CSVReader<IS>::getLine() noexcept
{
	return m_line;
}

template <typename IS>
void kr::CSVReader<IS>::skip()
{
	switch (m_nLast)
	{
	case eof: throw EofException();
	case '\n':
		m_nLast = ',';
		m_line++;
		throw NextLineException();
	case ',': break;
	}

	int finded = 0;

	try
	{
		while(m_stream.peek() == '\"')
		{
			m_stream.skip(1);
			m_stream.skipto('\"');
		}

		finded = m_stream.skipto_y(",\n");
	}
	catch (EofException&)
	{
		m_stream.clearBuffer();
		finded = eof;
	}

	m_nLast = finded;
}
template <typename IS>
template <typename Derived, typename C, typename _Info>
void kr::CSVReader<IS>::next(OutStream<Derived, _Info>* dest)
{
	switch (m_nLast)
	{
	case eof: throw EofException();
	case '\n':
		m_nLast = ',';
		m_line++;
		throw NextLineException();
	case ',': break;
	}

	int finded = 0;

	try
	{
		char chr = m_stream.peek();
		if (chr == '\"')
		{
			m_stream.skip(1);

			for (;;)
			{
				try
				{
					size_t pos = m_stream.pos('\"');
					if (m_stream.peek(pos + 1) != '\"')
					{
						m_stream.read(pos).writeTo(dest);
						m_stream.skip(1);
						break;
					}
					else
					{
						m_stream.read(pos + 1).writeTo(dest);
						m_stream.skip(1);
					}
				}
				catch (TooBigException&)
				{
					m_stream.text().writeTo(dest);
					m_stream.clearBuffer();
				}
			}
		}

		m_stream.readto_y(",\n", (char*)&finded).writeTo(dest);
	}
	catch (EofException&)
	{
		m_stream.text().writeTo(dest);
		m_stream.clearBuffer();
		finded = eof;
	}

	m_nLast = finded;
}
template <typename IS>
kr::TmpArray<typename IS::Component> kr::CSVReader<IS>::next()
{
	TText tx((size_t)0, 1024);
	next(&tx);
	return tx;
}
template <typename IS>
kr::Array<typename IS::Component> kr::CSVReader<IS>::nextAlloc()
{
	AText tx;
	next(&tx);
	return tx;
}

template <typename OS>
kr::CSVWriter<OS>::CSVWriter(nullptr_t) noexcept:m_stream(nullptr_t)
{
	m_prev = false;
}
template <typename OS>
kr::CSVWriter<OS>::CSVWriter(OS* stream) noexcept : m_stream(stream)
{
	m_prev = false;
}
template <typename OS>
kr::CSVWriter<OS>& kr::CSVWriter<OS>::operator = (nullptr_t) noexcept
{
	m_stream = nullptr;
	return *this;
}
template <typename OS>
kr::CSVWriter<OS>& kr::CSVWriter<OS>::operator = (OS* stream) noexcept
{
	m_stream = stream;
	return *this;
}
template <typename OS>
OS* kr::CSVWriter<OS>::operator &() const noexcept
{
	return m_stream;
}

template <typename OS>
template <typename Converter, typename C2>
void kr::CSVWriter<OS>::write(RefArray<C2> str) // NotEnoughSpaceException
{
	static const auto filter = meta::literal_as<C2>("\"\'\n\r,");
	static const auto from = meta::literal_as<C2>("\"");
	static const auto to = meta::literal_as<Component>("\"\"");

	if (m_prev) m_stream->write((Component)',');

	bool quote = (str.find_y(filter) != nullptr);
	if(quote) m_stream->write((Component)'\"');
	TmpArray<Component> temp;
	str.replace<Converter>(&temp, (RefArray<C2>)from, (RefArray<Component>)to);
	m_stream->write(temp);
	if (quote) m_stream->write((Component)'\"');
	m_prev = true;
}
template <typename OS>
void kr::CSVWriter<OS>::write(RefArray<Component> str) // NotEnoughSpaceException
{
	write<RefArray<Component>, Component>(str);
}
template <typename OS>
void kr::CSVWriter<OS>::nextLine() // NotEnoughSpaceException
{
	m_stream->write((Component)'\n');
	m_prev = false;
}
