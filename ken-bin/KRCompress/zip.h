#pragma once

#include <KR3/main.h>

struct tm_unz_s;

namespace kr
{
	class Unzipper
	{
	public:
		Unzipper();
		virtual ~Unzipper();

		bool open(pcwstr szFileName);
		int extract(pcwstr szDestination);
		void close();

	private:
		int _extractCurrentFile(pcwstr dest);
		int _writeFile(File * file) noexcept;
		void _changeFileDate(pcwstr filename, dword dosdate, const tm_unz_s* tmu_date);

	private:
		void * m_uzFile;
	};
}
