#pragma once

#include "../main.h"

namespace kr
{
	
	namespace meta
	{
		// maximum
		template <int v, int ... values> struct maximum
		{
			using super = maximum<values ...>;
			static constexpr int value = super::value > v ? super::value : v;
		};;
		template <int v> struct maximum<v>
		{
			static constexpr int value = v;
		};;

		// lastbit
		template <typename T> struct LastBitOf
		{
			static constexpr size_t value = 1 << (sizeof(T)*8-1);
		};
	
		// max of / over of
		template <typename T> struct NumberInfo {};

		template <> struct NumberInfo<qword>
		{
			static constexpr qword maxof = 0xffffffffffffffff;
			static constexpr qword minof = 0;
			static const float overof;
		};

		template <> struct NumberInfo<llong>
		{
			static constexpr llong maxof = 0x7fffffffffffffff;
			static constexpr llong minof = (llong)0x8000000000000000;
			static const float overof;
		};

		template <> struct NumberInfo<dword>
		{
			static constexpr dword maxof = 0xffffffff;
			static constexpr dword minof = 0;
			static const float overof;
		};

		template <> struct NumberInfo<long>
		{
			static constexpr long maxof = 0x7fffffff;
			static constexpr long minof = (long)0x80000000;
			static const float overof;
		};

		template <> struct NumberInfo<word>
		{
			static const word maxof = 0xffff;
			static const word minof = 0;
			static const float overof;
		};

		template <> struct NumberInfo<short>
		{
			static constexpr short maxof = 0x7fff;
			static constexpr short minof = (short)0x8000;
			static const float overof;
		};

		template <> struct NumberInfo<byte>
		{
			static const byte maxof = 0xff;
			static const byte minof = 0;
			static const float overof;
		};
		template <> struct NumberInfo<char>
		{
			static constexpr char maxof = 0x7f;
			static constexpr char minof = (char)0x80;
			static const float overof;
		};
		template <> struct NumberInfo<uint>
		{
			static constexpr uint maxof = 0xffffffff;
			static constexpr uint minof = 0;
			static const float overof;
		};
		template <> struct NumberInfo<int>
		{
			static constexpr int maxof = 0x7fffffff;
			static constexpr int minof = (int)0x80000000;
			static const float overof;
		};
	}


	template <typename TYPE, size_t alignment>
	struct alignedsize
	{
		static constexpr size_t value = (sizeof(TYPE) + alignment - 1) / alignment * alignment;
	};
}

#define lastbitof(type) (kr::meta::LastBitOf<type>::value)
#define maxof(type)	(kr::meta::NumberInfo<type>::maxof)
#define minof(type)	(kr::meta::NumberInfo<type>::minof)
#define overof(type) (kr::meta::NumberInfo<type>::overof)