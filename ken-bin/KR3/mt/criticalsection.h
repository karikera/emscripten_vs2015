#pragma once

#include <KR3/main.h>

namespace kr
{
	class CriticalSection;
	class CsLock
	{
	public:
		CsLock(CriticalSection & cs) noexcept;
		~CsLock() noexcept;

	private:
		CriticalSection * m_cs;
	};

	class CriticalSection final
	{
		friend CsLock;
	public:
		CriticalSection() noexcept;
		~CriticalSection() noexcept;
		void enter() noexcept;
		void leave() noexcept;

	protected:
#ifdef WIN32

#pragma pack(push, 8)
		struct CRITICAL_SECTION_DATA {
			void * DebugInfo;
			long LockCount;
			long RecursionCount;
			void* OwningThread;        // from the thread's ClientId->UniqueThread
			void* LockSemaphore;
			uintp SpinCount;        // force size on 64-bit systems when packed
		};
#pragma pack(pop)


		CRITICAL_SECTION_DATA m_cs;
#else
		pthread_mutex_t m_mutex;
#endif
	};

}
