#pragma once

#ifndef WIN32
#error is not windows system
#endif

#include "handle.h"
#include "eventhandle.h"

namespace kr
{
	enum class ThreadPriority
	{
		Idle,
		Lowest,
		Low,
		Normal,
		High,
		Highest,
	};
	class ThreadHandle;
	class MessageThreadId;

	class ThreadId
	{
		friend MessageThreadId;
		friend ThreadHandle;
	public:
		ThreadId() noexcept = default;
		ThreadId(nullptr_t) noexcept;
		void quit(int exitCode) noexcept;
		void setName(pcstr name) noexcept;
		static ThreadId getCurrent() noexcept;

		bool operator == (const ThreadId & id) const noexcept;
		bool operator != (const ThreadId & id) const noexcept;
		bool operator == (nullptr_t) const noexcept;
		bool operator != (nullptr_t) const noexcept;

	private:
		ThreadId(dword id) noexcept;

		dword m_id;
	};

	class ThreadHandle :public EventHandle
	{
	public:
		ThreadHandle() = delete;
		~ThreadHandle() = delete;

		template <typename T> using ThreadProc = int(CT_STDCALL *)(T*);

		template <typename T>
		static kr::ThreadHandle* createRaw(ThreadProc<T> fn, T * param, ThreadId * id = nullptr) noexcept;

		template <typename T, int(*fn)(T*)>
		static kr::ThreadHandle* create(T * param, ThreadId * id = nullptr) noexcept
		{
			return createRaw<T>([](T * param)->int
			{
				int ret = *fn(param);
				return ret;
			}, param, id);
		}
		template <typename This, int (This::*fn)()>
		static kr::ThreadHandle* create(This * param, ThreadId * id = nullptr) noexcept
		{
			return createRaw<This>([](This * param)->int
			{
				int ret = (param->*fn)();
				return ret;
			}, param, id);
		}
		template <typename This, int (This::*fn)() const>
		static kr::ThreadHandle* create(This * param, ThreadId * id = nullptr) noexcept
		{
			return createRaw<This>([](This*param)->int
			{
				int ret = param->*fn();
				return ret;
			}, param, id);
		}

		template <typename LAMBDA>
		static kr::ThreadHandle * createLambda(LAMBDA lambda, ThreadId * id = nullptr)
		{
			using NLAMBDA = meta::ChReturn<int, LAMBDA>;
			NLAMBDA * plambda = _new NLAMBDA(move(lambda));
			return createRaw<NLAMBDA>([](NLAMBDA * p)->int { int ret = (*p)(); delete p; return ret; }, plambda, id);
		}

#if WINVER >= 0x502
		ThreadId getId() noexcept;
#endif
		void suspend() noexcept;
		void terminate() noexcept;
		void detach() noexcept;
		void join() noexcept;
		void setPriority(ThreadPriority priority) noexcept;

		static ThreadHandle * getCurrent() noexcept;
	private:
		using EventHandle::set;
		using EventHandle::reset;
	};

}


template <typename T>
kr::ThreadHandle* kr::ThreadHandle::createRaw(ThreadProc<T> fn, T * param, ThreadId * id) noexcept
{
	return createRaw<void>((ThreadProc<void>)fn, param, id);
}
template <>
kr::ThreadHandle* kr::ThreadHandle::createRaw<void>(ThreadProc<void> fn, void * param, ThreadId * id) noexcept;
