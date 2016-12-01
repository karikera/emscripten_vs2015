#pragma once

#ifndef WIN32
#error is not windows system
#endif

#include <KR3/main.h>
#include "../fs/file.h"

typedef struct HINSTANCE__ *HINSTANCE;

namespace kr
{
	template <typename T> class Resource: public Resource<void>
	{
	public:
		using Resource<void>::Resource;
		inline operator T*()
		{
			return (T*)m_resource;
		}
	};
	template <> class Resource<void>
	{
	public:
		Resource(HINSTANCE hModule, int id, int type) noexcept;
		~Resource() noexcept;
		void * begin() noexcept;
		size_t size() noexcept;

	protected:
		void* m_rsrc;
		void* m_hGlobal;
		size_t m_size;
		void * m_resource;
	};

	class ResourceFile:public Resource<void>
	{
	public:
		ResourceFile(HINSTANCE hModule, int id, int type) noexcept;
		~ResourceFile() noexcept;
		bool toFile(pcwstr str, bool temp=false); // FileException
		int execute(TextW param) noexcept;

	protected:
		static int m_nTempNo;
		BTextW<File::NAMELEN> m_strFileName;
		bool m_isTemp;
	};


}