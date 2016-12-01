#pragma once

#include <string>
#include <vector>
#include <list>
#include <unordered_map>


using namespace std;

class Error
{
public:
	Error(const char * msg, ...);
	void print();

private:
	char m_message[1024];
};

extern string g_filename;
extern int g_line;

void must(char chr, char must);
