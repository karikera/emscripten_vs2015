
#pragma once

#include "ncstrlib.h"

inline size_t CRtoCRLF(char * out,const char * str,size_t limit)
{
	char * out_end = out + limit-1;
	char * out_start = out;

	LPCSTR end = strchr(str,'\0');
	LPCSTR prev = str,next;

	while(next = (char*)memchr(prev,'\n',end-prev))
	{
		size_t skip = next-prev;
		if(out+skip+2 >= out_end)
		{
			*out = '\0';
			return out - out_start;
		}

		memcpy(out,prev,skip);
		out += skip;
		*(out++) = '\r';
		*(out++) = '\n';
		
		prev = next+1;
	}
	memcpy(out,prev,end-prev+1);
	
	return out-prev +end-out_start;
}

inline size_t addslashes(char * out,const char * str,size_t limit)
{
	char * out_end = out + limit -2;
	char * out_start = out;
	const char * end = strchr(str,'\0');
	while(str != end)
	{
		if(out >= out_end)
		{
			*out = '\0';
			return out - out_start;
		}

		char chr = *(str++);
		switch(chr)
		{
		case '\r':
			*(out++) = '\\';
			*(out++) = 'r';
			break;
		case '\n':
			*(out++) = '\\';
			*(out++) = 'n';
			break;;
		case '\t':
			*(out++) = '\\';
			*(out++) = 't';
			break;
		case '\\':
			*(out++) = '\\';
			*(out++) = '\\';
			break;
		case '\'':
			*(out++) = '\\';
			*(out++) = '\'';
			break;
		case '\"':
			*(out++) = '\\';
			*(out++) = '\"';
			break;
		default:
			*(out++) = chr;
			break;
		}
	}
	*out = '\0';
	return out - out_start;
}
inline size_t unslashes(char * out,const char * str,size_t limit)
{
	char * out_end = out + limit -1;
	char * out_start = out;
	const char * end = strchr(str,'\0');
	while(str != end)
	{
		if(out == out_end)
		{
			*out = '\0';
			return out - out_start;
		}

		if(*str != '\\')
		{
			*(out++) = *(str++);
			continue;
		}
		str++;
		switch(*(str++))
		{
		case 'R':
		case 'r': *(out++) = '\r'; break;
		case 'N':
		case 'n': *(out++) = '\n'; break;;
		case 'T':
		case 't': *(out++) = '\t'; break;
		case '\\': *(out++) = '\\'; break;
		case '\'': *(out++) = '\''; break;
		case '\"': *(out++) = '\"'; break;
		}
	}
	*out = '\0';
	return out - out_start;
}