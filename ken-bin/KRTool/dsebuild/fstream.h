#pragma once

#include <KR3/main.h>
#include "common.h"
#include <fstream>

class Error;
class InFile;
class OutFile;

extern InFile g_is;
extern OutFile g_os;

using kr::Text;

class InFile
{
public:
	InFile();
	void open(const string & fname);
	void close();
	bool fail();

	char get();
	void unget();
	char peek();

	char skipLine();
	char skipComments();
	char skipSpace();
	void skipSpace(char chr);
	char readWord(std::string * out);
	string readWord(char endCode);
	int readNumber(char endCode);
	template <size_t size> char skipSpace(const char(&_mask)[size]);
	template <size_t size> char readWord(std::string * out, const char(&_mask)[size]);
	template <size_t size, typename LAMBDA> char readWord(LAMBDA lambda, const char(&_mask)[size]);

private:
	ifstream m_is;
};

class OutFile
{
public:
	OutFile();
	void open(const string & fname);
	void close();
	bool fail();
	void indent(int n);
	OutFile& operator <<(char chr);
	OutFile& operator <<(const char * str);
	OutFile& operator <<(Text chr);
	OutFile& operator <<(std::ostream&(CT_CDECL *f)(std::ostream&));

private:
	void _indentTest();
	ofstream m_os;
	bool m_endl;
	size_t m_indent;

};


template <typename LAMBDA, size_t size> char filter(LAMBDA lambda, const char(&_mask)[size])
{
	char det = lambda();
	if(memchr(_mask, det, size-1) == nullptr)
	{
		throw Error("Invalid character \'%c\' need '%s'", det, _mask);
	}
	return det;
}
template <typename LAMBDA> void filter(LAMBDA lambda, char _det)
{
	must(lambda(), _det);
}
template <size_t size> char InFile::skipSpace(const char(&_mask)[size])
{
	return filter([this] { return skipSpace(); }, _mask);
}
template <size_t size> char InFile::readWord(std::string * out, const char(&_mask)[size])
{
	return filter([&]{ return readWord(out); }, _mask);
}
template <size_t size, typename LAMBDA> char InFile::readWord(LAMBDA lambda, const char(&_mask)[size])
{
	return filter([&]
	{
		string out;
		char chr = readWord(&out);
		lambda(out);
		return chr;
	}, _mask);
}
