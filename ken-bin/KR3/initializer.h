#pragma once

#include "stdafx.h"

namespace kr
{
#ifdef WIN32
	struct COM
	{
		struct Init
		{
			Init() noexcept;
			~Init() noexcept;
		};
	};
	struct OLE
	{
		struct Init
		{
			Init() noexcept;
			~Init() noexcept;
		};
	};
	struct CommonControls
	{
		struct Init
		{
			Init() noexcept;
			~Init() noexcept;
		};
	};
	struct GdiPlus
	{
		struct Init
		{
			Init() noexcept;
			~Init() noexcept;

			uintp token;
		};
	};
#endif

	namespace _pri_
	{
		template <typename ... NEXTS>
		struct Initializer_2;

		template <typename T, bool isempty, typename ... NEXTS>
		struct Initializer_1;

		template <typename NEXT, typename ... NEXTS>
		struct Initializer_2<NEXT, NEXTS ...> : Initializer_1<NEXT, std::is_empty<NEXT>::value, NEXTS ...>
		{
		};
		template <>
		struct Initializer_2<>
		{
		};

		template <typename T, typename ... NEXTS>
		struct Initializer_1<T, true, NEXTS ...>: Initializer_2<NEXTS ...>
		{
			Initializer_1() noexcept
			{
				new((T*)this) T;
			}
			~Initializer_1() noexcept
			{
				((T*)this)->~T();
			}
		};
		template <typename T, typename ... NEXTS>
		struct Initializer_1<T, false, NEXTS ...> : Initializer_2<NEXTS ...>
		{
			T init;
		};
	}

	template <typename ... Inits>
	class Initializer: _pri_::Initializer_2<typename Inits::Init ...>
	{
	};
}

#ifdef WIN32

#ifdef _CONSOLE
#define main() CT_CDECL main() 
#else
#define main() APIENTRY wWinMain(HINSTANCE,HINSTANCE,kr::pstr16,int)
#endif

#else

#define main() main() 

#endif
