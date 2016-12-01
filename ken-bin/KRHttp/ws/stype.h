
#pragma once
#include <cstddef>
#include <memory.h>
#include <inttypes.h>

static class CInvalid {} invalid;

typedef uint32_t IPADDRESS;

inline void strcpys(char * dest,const char * src,size_t limit)
{
    size_t len = strlen(src);
    if(len >= limit) len = limit-1;
    memcpy(dest,src,len);
    dest[len] ='\0';
}