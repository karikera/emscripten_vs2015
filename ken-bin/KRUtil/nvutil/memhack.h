#pragma once

#ifndef WIN32
#error is not windows system
#endif

#include <KR3/wl/windows.h>

namespace kr
{

	class HackProcess;
	typedef void (CALLBACK * DEBUGPROC)(LPVOID pParam, DEBUG_EVENT *de);

	class ProcessMemory
	{
	public:
		ProcessMemory(HANDLE hProcess);
		ProcessMemory(HANDLE hProcess, size_t nSize, dword dwFlags);
		~ProcessMemory();

		bool alloc(size_t nSize, dword dwFlags);
		size_t write(LPCVOID pData, size_t nSize);
		size_t read(LPVOID pData, size_t nSize);
		LPVOID getAddress();

	protected:
		HANDLE m_hProcess;
		LPVOID m_pAddress;
		size_t m_nSize;
	};

	class HackProcess
	{
		friend UINT CALLBACK debugThread(LPVOID pThis);
	public:
		HackProcess();
		HackProcess(dword dwID);
		~HackProcess();
		HackProcess(const HackProcess &copy) = delete;
		HackProcess(HackProcess &&move);

		HANDLE getProcessHandle();
		HMODULE injectDll(LPCSTR strDllPath);
		HMODULE injectDll(LPCWSTR strDllPath);
		dword call(LPTHREAD_START_ROUTINE pThread, LPCVOID pParam, size_t nSize);
		void open(dword dwID);
		void close();
		bool execute(LPSTR strCommand, LPCSTR strPath = nullptr);
		bool execute(LPWSTR strCommand, LPCWSTR strPath = nullptr);
		void terminate();
		void resume();
		void wait();
		void debugStart(DEBUGPROC proc, LPVOID pParam);
		void debugStop();
		void debugContinue(dword dwEvent);
		size_t write(void * pDest, const void *pSrc, size_t nSize);
		size_t read(void * pDest, const void *pSrc, size_t nSize);

		void debugCallback(DEBUG_EVENT *pde);

	protected:
		PROCESS_INFORMATION m_pi;
		DEBUGPROC m_lpfnDebug;
		LPVOID m_pParam;
		dword m_dwWaitThread;
	};

	bool getProcessName(LPTSTR strName, dword dwLen, HANDLE hProcess);
	HackProcess getProcessByName(LPCTSTR strName);
	void getProcessByName(Array<HackProcess> &list, LPCTSTR strName);
}
