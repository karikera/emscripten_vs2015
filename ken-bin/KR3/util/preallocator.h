#pragma once

#include "../main.h"

#include <new>

namespace kr
{
	template <typename T, size_t COUNT> class ObjKeeper
	{
	public:
		ObjKeeper() noexcept
		{
			m_reserve = 0;
		}
		~ObjKeeper() noexcept
		{
			clear();
		}
		void* rawAlloc() noexcept
		{
			void * data;
			if (m_reserve != 0)
			{
				m_reserve--;
				data = m_reserved[m_reserve];
#ifndef NDEBUG
				memset(data, 0xcc, sizeof(T));
#endif
			}
			else
			{
				data = _new char[sizeof(T)];
			}
			return data;
		}
		void rawFree(void * p) noexcept
		{
			if (m_reserve != COUNT)
			{
#ifndef NDEBUG
				memset(p, 0xdd, sizeof(T));
#endif
				m_reserved[m_reserve] = p;
				m_reserve++;
				return;
			}

			delete[](char*)p;
		}
		template <typename ... ARGS> T * alloc(ARGS && ... args) noexcept
		{
			return ::new(rawAlloc()) T(args ...);
		}
		void free(T * ptr) noexcept
		{
			ptr->~T();
			rawFree(ptr);
		}
		void clear()
		{
			void ** p = m_reserved;
			void ** end = p + m_reserve;
			for(; p != end; p++)
			{
				delete [] (char*)*p;
			}
			m_reserve = 0;
		}

	private:
		size_t m_reserve;
		void* m_reserved[COUNT];
	};


	template <size_t SIZE, size_t COUNT> class SharingKeeper
	{
	public:
		SharingKeeper() noexcept
		{
			m_reserve = 0;
		}
		~SharingKeeper() noexcept
		{
			clear();
		}
		void* rawAlloc(size_t sz) noexcept
		{
			_assert(sz <= SIZE);

			void * data;
			if (m_reserve != 0)
			{
				m_reserve--;
				data = m_reserved[m_reserve];
#ifndef NDEBUG
				memset(data, 0xcc, sz);
#endif
			}
			else
			{
				data = _new byte[SIZE];
			}
			return data;		
		}
		void rawFree(void * p) noexcept
		{
			if (m_reserve != COUNT)
			{
#ifndef NDEBUG
				memset(p, 0xdd, kr_msize(p));
#endif
				m_reserved[m_reserve] = p;
				m_reserve++;
				return;
			}

			delete[](byte*)p;
		}
		template <typename T, typename ... ARGS>
		T * alloc(ARGS && ... args) noexcept
		{
			static_assert(sizeof(T) <= SIZE, "Too big size class");

			return ::new(rawAlloc(sizeof(T))) T(args ...);
		}
		template <typename T>
		void free(T * ptr) noexcept
		{
			ptr->~T();
			rawFree(ptr);
		}
		void clear()
		{
			void ** p = m_reserved;
			void ** end = p + m_reserve;
			for (; p != end; p++)
			{
				delete[](byte*)*p;
			}
			m_reserve = 0;
		}

	private:
		size_t m_reserve;
		void* m_reserved[COUNT];
	};


}
