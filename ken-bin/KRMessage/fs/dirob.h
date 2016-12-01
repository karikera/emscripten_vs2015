#pragma once

#include <KR3/main.h>
#include <KR3/mt/thread.h>
#include <KRUtil/fs/file.h>

namespace kr
{
	class DirectoryObserver :private Threadable<DirectoryObserver>
	{
		friend Threadable<DirectoryObserver>;
	public:
		DirectoryObserver() noexcept;
		~DirectoryObserver() noexcept;
		void start(const wchar * dir); // FileException
		void quit(int exitCode) noexcept;
		int thread() noexcept;
		virtual void onCreate(TextW name) noexcept = 0;
		virtual void onDelete(TextW name) noexcept = 0;
		virtual void onModified(TextW name) noexcept = 0;
		virtual void onRename(TextW oldname, TextW newname) noexcept = 0;

	private:
		File * m_dir;
		dword m_id;
	};
}
