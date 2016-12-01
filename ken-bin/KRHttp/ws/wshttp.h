
#pragma once

#include "sockstream.h"

enum class WSOpcode:uint8_t
{
    Continue,     // 0: continuation frame
    Text,         // 1: text frame
    Binary,       // 2: binary frame
                    // 3~7: reserved.
    Close=8,      // 8: connection close
    Ping,         // 9: ping
    Pong,         // A: pong
                    // B~F: reserved.
};
#pragma pack(push)
#pragma pack(1)

struct WS_FRAME
{
    WSOpcode Opcode:4;
    bool RSV3:1; // Reserved.
    bool RSV2:1; // Reserved.
    bool RSV1:1; // Reserved.
    bool FIN:1; // Final fragment
    
    uint8_t Length:7;
    bool Mask:1; // Mask data.
    // 0-125: Length
    // 126: After 2bytes Length
    // 127: After 8bytes Length
    
    inline size_t GetLengthExtend()
    {
        if(Length < 126) return 0;
        if(Length == 126) return 2;
        if(Length == 127) return 8;
    }
    inline size_t GetMaskExtend()
    {
        return (Mask ? 4 : 0);
    }
    inline size_t GetExtendSize()
    {
        return GetLengthExtend() + GetMaskExtend();
    }
    inline size_t GetSize()
    {
        return sizeof(WS_FRAME) + GetExtendSize();
    }
    inline uint64_t GetDataLength()
    {
        if(Length < 126) return Length;
        if(Length == 126) return endianReverse16(*(uint16_t*)(this+1));
        if(Length == 127) return endianReverse64(*(uint64_t*)(this+1));
    }
    void * GetMask()
    {
        return ((char*)(this+1)+GetLengthExtend());
    }
    void * GetData()
    {
        return ((char*)(this+1)+GetExtendSize());
    }
    inline void SetDataLengthAuto(uint64_t len)
    {
        if(len < 126)
        {
            SetDataLength7((uitn)len);
        }
        else if(len < 65536)
        {
            SetDataLength16(len);
        }
        else
        {
            SetDataLength64(len);
        }
    }
    inline void SetDataLength7(unsigned len)
    {
        Length = len;
    }
    inline void SetDataLength16(unsigned len)
    {
        Length = 126;
        *(uint16_t*)(this+1) = endianReverse16(len);
    }
    inline void SetDataLength64(uint64_t len)
    {
        Length = 127;
        *(uint64_t*)(this+1) = endianReverse64(len);
    }
    void SetMask(void * dat)
    {
        Mask = true;
        memcpy(GetMask(),dat,4);
    }
};

struct WS_FRAME_EX:WS_FRAME
{
    uint64_t LengthEx;
    uint8_t MaskEx[4];
};

#pragma pack(pop)

struct HTTP_HEADER
{
    char lpszHeader[256];
    char lpszHost[256];
    char lpszOrizin[256];
    char lpszWSKey[256];
    char lpszProtocol[256];
    int nWSVersion;
    
    const char * Read(CSocketStream & sock);
    void GetAcceptKey(char * dest,size_t limit);
    void Print();
    
};
