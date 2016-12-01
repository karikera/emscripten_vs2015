#pragma once

#include <vector>
#include <initializer_list>

#include "sockstream.h"

class CWebSocketPath;
class CWebSocketPage;

class CWebSocketPath
{
public:
    CWebSocketPath();
    CWebSocketPath(std::initializer_list<CWebSocketPage*> list);
    CWebSocketPage* Get(const char * strPath,size_t maxlen);
    bool Call(CSocketStream &sock,const char * strPath);
    void Add(CWebSocketPage *page);
  
protected:
    std::vector<CWebSocketPage*> m_vSubPage;
};

class CWebSocketPage:public CWebSocketPath
{
    friend class CWebSocketPath;
public:
    CWebSocketPage(const char * strName);
    virtual void OnPageCall(CSocketStream &sock) = 0;
    
protected:
    const char * m_strName;
};