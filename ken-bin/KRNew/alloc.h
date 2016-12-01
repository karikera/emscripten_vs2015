#pragma once

#include <KR3/common/type.h>
#include <KR3/common/autovar/autoptr.h>

#ifndef NDEBUG
#define KR_MEMORY_OBSERVER
#endif

#ifdef KR_MEMORY_OBSERVER
#define _new new(__FILE__ , __LINE__)

void * CT_CDECL operator new(size_t size, const char * file, int line) noexcept;
void * CT_CDECL operator new[](size_t size, const char * file, int line) noexcept;
void CT_CDECL operator delete(void * data, const char * file, int line) noexcept;
void CT_CDECL operator delete[](void * data, const char * file, int line) noexcept;

inline void * CT_FASTCALL getAllocatedPointer(void * value) noexcept
{
	return value;
}

namespace kr
{
	namespace _pri_
	{
		void CT_FASTCALL reline_new_impl(void * ptr, const char * file, int line) noexcept;
		void CT_FASTCALL depend_new_impl(void * parent, void * child) noexcept;
		template <typename T>
		T& reline_new_pass_impl(T& ptr, const char * file, int line) noexcept
		{
			reline_new_impl(ptr, file, line);
			return ptr;
		}
		template <typename T>
		T* reline_new_pass_impl(T* ptr, const char * file, int line) noexcept
		{
			reline_new_impl(ptr, file, line);
			return ptr;
		}
	}
}
#define reline_new(ptr) (::kr::_pri_::reline_new_pass_impl(ptr, __FILE__, __LINE__))
#define depend_new(parent, child) (::kr::_pri_::depend_new_impl(\
		getAllocatedPointer(parent), \
		getAllocatedPointer(child)))

#else

#define _new new
#define _quiet_new new
#define reline_new(ptr)	(ptr)
#define depend_new(parent, child) do {} while(0,0)

#endif

void * CT_CDECL operator new(size_t size);
void * CT_CDECL operator new[](size_t size);
void CT_CDECL operator delete(void * data) noexcept;
void CT_CDECL operator delete[](void * data) noexcept;


#ifdef KR_MEMORY_OBSERVER
#define kr_alloc(sz)				((::kr::autoptr)reline_new(::kr::_pri_::alloc_impl((sz))))
#define kr_aligned_alloc(sz, ...)	((::kr::autoptr)reline_new(::kr::_pri_::aligned_alloc_impl((sz), __VA_ARGS__)))
#else
#define kr_alloc(sz)				((::kr::autoptr)::kr::_pri_::alloc_impl((sz)))
#define kr_aligned_alloc(sz, ...)	((::kr::autoptr)::kr::_pri_::aligned_alloc_impl((sz), __VA_ARGS__))
#endif
#define kr_free(ptr)			(::kr::_pri_::free_impl(ptr))
#define kr_expand(ptr,sz)		(::kr::_pri_::expand_impl(ptr,sz))
#define kr_msize(ptr)			(::kr::_pri_::msize_impl(ptr))
#define kr_aligned_free(ptr)	(::kr::_pri_::aligned_free_impl(ptr))
#define kr_aligned_msize(ptr, ...) (::kr::_pri_::aligned_msize_impl(ptr, __VA_ARGS__))

namespace kr
{
	namespace _pri_
	{
		ATTR_CHECK_RETURN void* CT_FASTCALL alloc_impl(size_t sz) noexcept;
		ATTR_CHECK_RETURN bool CT_FASTCALL expand_impl(void * data, size_t nsize) noexcept;
		void CT_FASTCALL free_impl(void * data) noexcept;
		ATTR_CHECK_RETURN size_t CT_FASTCALL msize_impl(const void * ptr) noexcept;

		ATTR_CHECK_RETURN void* CT_FASTCALL aligned_alloc_impl(size_t sz, size_t aligned) noexcept;
		ATTR_CHECK_RETURN void* CT_FASTCALL aligned_alloc_impl(size_t sz, size_t aligned, size_t offset) noexcept;
		ATTR_CHECK_RETURN size_t CT_FASTCALL aligned_msize_impl(const void * ptr, size_t aligned) noexcept;
		ATTR_CHECK_RETURN size_t CT_FASTCALL aligned_msize_impl(const void * ptr, size_t aligned, size_t offset) noexcept;
		void CT_FASTCALL aligned_free_impl(void * ptr) noexcept;

		template <typename T, typename ... ARGS> T * newAligned(const ARGS &... args)
		{
			T* data = (T*)_pri_::aligned_alloc_impl(sizeof(T), alignof(T));
			return new(data) T(args...);
		}
		template <typename T, typename ... ARGS> T * newAlignedArray(size_t sz)
		{
			static_assert(sizeof(T) % alignof(T) == 0, "sizeof & alignment unmatch!!");

			if (std::is_class<T>::value)
			{
				size_t * cnt = (size_t*)_pri_::aligned_alloc_impl(sizeof(T) * sz, alignof(T), sizeof(size_t));
				*cnt = sz;
				T * data = (T*)(cnt + 1);
				T * end = data + sz;
				for (T * beg = data; beg != end; beg++)
				{
					new (beg) T;
				}
				return data;
			}
			else
			{
				return (T*)_pri_::aligned_alloc_impl(sizeof(T) * sz, alignof(T));
			}
		}
		template <typename T, typename ... ARGS> T * newAlignedArrayNC(size_t sz)
		{
			static_assert(sizeof(T) % alignof(T) == 0, "sizeof & alignment unmatch!!");

			return (T*)_pri_::aligned_alloc_impl(sizeof(T) * sz, alignof(T));
		}
	}

	// 메모리 테스트의 지점을 정한다.
	// 디버그 모드에서만 동작한다.
	void CT_FASTCALL memtest_set_axis() noexcept;

	// 정한 지점만큼 메모리가 돌아와있는지 테스트 한다.
	// 디버그 모드에서만 동작한다.
	void CT_FASTCALL memtest() noexcept;

	// 할당된 메모리를 전부 지운다.
	// 디버그 모드에서만 동작한다.
	// 특수한 경우 외에는 사용하지 말 것.
	void CT_FASTCALL memdestroy() noexcept;

	// 전체 메모리를 오염되었는지 확인한다.
	void CT_FASTCALL memcheck() noexcept;

	struct MemoryTest
	{
		inline MemoryTest() noexcept{ memtest_set_axis(); }
		inline ~MemoryTest() noexcept{ memtest(); }
	};

	template <typename T> void deleteAligned(T * ptr) noexcept
	{
		if (ptr == nullptr) return;
		ptr->~T();
		_pri_::aligned_free_impl(ptr);
	}
	template <typename T> void deleteAlignedND(T * ptr) noexcept
	{
		if (ptr == nullptr) return;
		_pri_::aligned_free_impl(ptr);
	}
	template <typename T> void deleteAlignedArray(T * ptr) noexcept
	{
		if (ptr == nullptr) return;
		if (std::is_class<T>::value)
		{
			size_t * cnt = (size_t*)ptr - 1;
			
			T * end = ptr + *cnt;
			for (; ptr != end; ptr++)
			{
				ptr->~T();
			}
			_pri_::aligned_free_impl(cnt);
		}
		else
		{
			_pri_::aligned_free_impl(ptr);
		}
	}
}


#define _newAligned(T, ...) new((T*)kr_aligned_alloc(sizeof(T), alignof(T))) T(__VA_ARGS__)
#define _newAlignedArray(T, cnt) reline_new(::kr::_pri_::newAlignedArray<T>(cnt))
#define _newAlignedArrayNC(T, cnt) reline_new(::kr::_pri_::newAlignedArrayNC<T>(cnt))