#pragma once

#ifndef WIN32
#error is not windows system
#endif

#include <KR3/main.h>
#include <KR3/wl/windows.h>

#include <Unknwn.h>
#include <kr3/meta/types.h>

namespace kr
{

#define OVERRIDE_UNKNOWN() \
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** ppv) override{ return ComImplement::QueryInterface(riid,ppv); };\
	virtual ULONG STDMETHODCALLTYPE AddRef() override{ return ComImplement::AddRef(); };\
	virtual ULONG STDMETHODCALLTYPE Release() override{ return ComImplement::Release(); };

	template <typename T> HRESULT createCOMObject(T ** p, REFCLSID rclsid, LPUNKNOWN pUnkOuter = nullptr, DWORD dwClsContext = CLSCTX_ALL)
	{
		return CoCreateInstance(rclsid, pUnkOuter, dwClsContext, __uuidof(T), (void**)p);
	}

	template <class T, class ... ARGS> class ComImplement:public ARGS...
	{
	protected:
		IUnknown* _GetUnknown()
		{
			return static_cast<IUnknown*>(this);
		}

		dword m_nReference;

		template <typename T> int _queryInterface(REFIID riid, void ** ppv)
		{
			if (__uuidof(T) == riid)
			{
				*ppv = static_cast<T*>(this);
				AddRef();
				throw (int)0;
			}
			return 0;
		}

	public:
		ComImplement()
		{
			m_nReference = 1;
		}
		
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** ppv) override
		{
			if(ppv == nullptr) return E_POINTER;
			try
			{
				unpackR(_queryInterface<ARGS>(riid, ppv));
				return E_NOINTERFACE;
			}
			catch(int)
			{
				return NOERROR;
			}
		}
		virtual ULONG STDMETHODCALLTYPE AddRef() override
		{
			return InterlockedIncrement(&m_nReference);
		}
		virtual ULONG STDMETHODCALLTYPE Release() override
		{
			UINT nRef = InterlockedDecrement(&m_nReference);
			if(nRef == 0) delete static_cast<T*>(this);
			return nRef;
		}
	};

}

#ifdef NDEBUG
#define hrmustbe(x) {HRESULT __hr; if(FAILED(__hr = (x))) exit(__hr); }
#define hrshouldbe(x) (SUCCEEDED(x))
#define hrexcept(x)	{HRESULT __hr; if(FAILED(__hr = (x))) throw ::kr::ErrorCode(__hr); }
#else
#define hrmustbe(x) {\
	HRESULT hr = (x);\
	if(FAILED(hr)) { ::kr::error(#x "\r\nHRESULT: 0x%08X",hr); }\
}
#define hrshouldbe(x) ([&]{\
	HRESULT hr = (x);\
	if(FAILED(hr)){\
		::kr::dout << #x << ::kr::endl; \
		::kr::dout << __FILE__ << '(' << __LINE__ << ')' << ::kr::endl; \
		::kr::dout << "HRESULT: 0x" << ::kr::hexf(hr, 8) << ::kr::endl; \
		return false; \
	}\
	return true;}())

#define hrexcept(x) {\
HRESULT hr = (x);\
 if(FAILED(hr)) {\
	::kr::dout << #x << ::kr::endl; \
	::kr::dout << __FILE__ << '(' << __LINE__ << ')' << ::kr::endl; \
	::kr::dout << "HRESULT: 0x" << ::kr::hexf(hr, 8) << ::kr::endl; \
	throw ::kr::ErrorCode(hr);\
 } }
#endif