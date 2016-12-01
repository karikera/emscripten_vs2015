#pragma once

#include <KR3/main.h>
#include <atomic>
#include <KR3/data/linkedlist.h>
#include <KR3/util/callable.h>
#include <KR3/mt/thread.h>
#include <KR3/mt/criticalsection.h>

namespace kr
{
	class ThreadPool;
	using ThreadWork = Node<Callable>;

	class ThreadPool
	{
	public:
		ThreadPool() noexcept;
		~ThreadPool() noexcept;
		ThreadPool(const ThreadPool&)=delete;
		ThreadPool& operator =(const ThreadPool&) = delete;
		void post(ThreadWork * work) noexcept;

		template <typename LAMBDA>
		void work(LAMBDA lambda);

	private:
		int _thread() noexcept;
		Array<ThreadHandle*> m_threads;
		Chain<Callable> m_works;
		Event m_event;
		CriticalSection m_cs;
		
		std::atomic<uint> m_leftWorks;
		bool m_close;
	};

	template <typename LAMBDA>
	void ThreadPool::work(LAMBDA lambda)
	{
		struct Data:ThreadWork
		{
			LAMBDA m_lambda;

			Data(LAMBDA lamb)
				:m_lambda(std::move(lamb))
			{
			}
			void work() noexcept override
			{
				m_lambda();
			}
			void remove() noexcept override
			{
				delete this;
			}
		};
		post(_new Data(std::move(lambda)));
	}
}