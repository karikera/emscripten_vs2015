
#pragma once

#include <cstdlib>
#include <string.h>
#include <iostream>
#include <inttypes.h>
#include <memory.h>

inline char* clone(const char * str)
{
    size_t size = strlen(str)+1;
    char * nstr = new char[size];
    memcpy(nstr,str,size);
    return nstr;
}

using std::cout;
using std::endl;

static class CInvalid {} invalid;

typedef uint32_t IPADDRESS;

inline size_t strcpys(char * dest,const char * src,size_t limit)
{
    size_t len = strlen(src);
    if(len >= limit) len = limit-1;
    memcpy(dest,src,len);
    dest[len] ='\0';
    return len;
}

inline size_t strcats(char * dest,const char * src,size_t limit)
{
    size_t dlen = strlen(dest);
    limit -= dlen;
    dest += dlen;
    return strcpys(dest,src,limit)+dlen;
}

static class NULLPTR
{
public:
    template <typename T> operator T*()
    {
        return (T*)0;
    }
} nullptr_variable;
#define nullptr (nullptr_variable)