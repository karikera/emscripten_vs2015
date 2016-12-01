#pragma once

#include <KRUtil/fs/file.h>
#include <KRMessage/progressor.h>

namespace kr
{
	class Decompress7z
	{
	public:
		void decompress(File* file);
		virtual void onFileRead(uint readcount, uint fullcount) noexcept;

	private:
	};
	void unzip7z(pcstr filename, Progressor * prog);
	void unzip7z(pcwstr filename, Progressor * prog);
	void unzip7z(File * file, Progressor * prog);
}