#pragma once

#include <KR3/wl/windows.h>
#include <KR3/mt/thread.h>
#include <KR3/util/time.h>

namespace kr
{
	class ProgressNotifier
	{
	public:
		ProgressNotifier() noexcept;
		~ProgressNotifier() noexcept;

		void set(llong pr) noexcept;
		void set(llong pr, llong length) noexcept;
		void setEnd() noexcept;
		llong progress() noexcept;
		llong length() noexcept;

		virtual void onProgress() noexcept = 0;

	protected:
		llong m_progress;
		llong m_length;

	private:
		timepoint m_start;

	};
	class Progressor:private ProgressNotifier
	{
	public:
		Progressor() noexcept;
		~Progressor() noexcept;

		bool closed() noexcept;
		void start() noexcept;
		template <typename LAMBDA>
		bool postLambda(LAMBDA lambda)
		{
			return m_thread->postLambda(lambda);
		}
		void quit(int exitCode) noexcept;
		void join() noexcept;
		llong getProgress() noexcept;
		llong getLength() noexcept;
		void setProgress(llong progress) noexcept;
		void setProgress(llong progress, llong length) noexcept;
		virtual int progressor() noexcept = 0;
		virtual void onProgress() noexcept override = 0;

	protected:
		void _checkPoint(); // QuitException
		void _checkPoint(long long nProgress); // QuitException

	private:
		union
		{
			ThreadHandle * m_thread;
			EventHandle * m_readyEvent;
		};
		ThreadId m_threadId;

		static int CT_STDCALL _thread(Progressor* _this) noexcept;
	};

}
