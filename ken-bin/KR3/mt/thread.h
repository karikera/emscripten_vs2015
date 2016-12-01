#pragma once

#include "../main.h"
#include "../meta/chreturn.h"

#ifdef WIN32
#include "../wl/threadhandle.h"
#else
#include <pthread.h>
#endif

namespace kr
{
	template <typename LAMBDA> class LambdaThread;

	template <typename T> class Threadable
	{
	private:
#ifdef WIN32
		ThreadHandle* m_handle;
		ThreadId m_id;
#else
		pthread_t m_handle;
#endif

	public:
		inline Threadable() noexcept
		{
			m_handle = nullptr;
#ifdef WIN32
			m_id = nullptr;
#endif
		}
		inline ~Threadable() noexcept
		{
		}
		inline void start() noexcept
		{
#ifdef WIN32
			m_handle = ThreadHandle::create<T, &T::thread>(static_cast<T*>(this));
#else
			int pthread_create_res = pthread_create(&m_handle, nullptr, [](void * _this)->void* {
				return (void*)(intp)(int)((T*)(_this))->thread();
			}, this);
			_assert(pthread_create_res != 0);
#endif
		}
		inline void terminate() noexcept
		{
			if (m_handle != nullptr)
			{
#ifdef WIN32
				m_handle->terminate();
#else
				pthread_cancel(m_handle);
#endif
				m_handle = nullptr;
			}
		}

#ifdef WIN32
		inline ThreadHandle * getThread() const noexcept
		{
			return m_handle;
		}
		inline ThreadId getThreadId() const noexcept
		{
			return m_id;
		}
#endif

		inline void join() noexcept
		{
#ifdef WIN32
			m_handle->join();
#else
			pthread_join(m_handle);
#endif
			m_handle = nullptr;
		}
		inline bool exists() noexcept
		{
			return m_handle != nullptr;
		}
	};

	class Thread:public Threadable<Thread>
	{
	public:
		Thread() noexcept;
		virtual ~Thread() noexcept;
		virtual int thread() noexcept = 0;

		template <typename LAMBDA>
		static LambdaThread<LAMBDA>* wrap(LAMBDA lambda) noexcept;
	};
	template <typename LAMBDA> class LambdaThread:public Thread
	{
	public:
		LambdaThread(LAMBDA lambda);
		~LambdaThread() override;
		int thread() noexcept override;
	private:
		LAMBDA m_lambda;
	};
}

template <typename LAMBDA>
kr::LambdaThread<LAMBDA>* kr::Thread::wrap(LAMBDA lambda) noexcept
{
	return _new LambdaThread<LAMBDA>(move(lambda));
}
template <typename LAMBDA>
kr::LambdaThread<LAMBDA>::LambdaThread(LAMBDA lambda)
	:m_lambda(move(lambda))
{
}
template <typename LAMBDA>
kr::LambdaThread<LAMBDA>::~LambdaThread()
{
}
template <typename LAMBDA>
int kr::LambdaThread<LAMBDA>::thread() noexcept
{
	return meta::chreturn<int>(m_lambda)();
}