#pragma once

#include "stream.h"

namespace kr
{
	namespace io
	{
		template <typename OS>
		class XorOStream:public OutStream<XorOStream<OS>, StreamInfo<typename OS::Component>>
		{
			static_assert(IsOStream<OS>::value, "OS is not OutStream");

		private:
			OS * m_os;
			dword m_key;
			dword m_add;

		public:
			XorOStream(OS * os, dword key, dword add = 3508816465) noexcept
				:m_os(os), m_key(key), m_add(add)
			{
			}

			void writeImpl(typename const OS::Component * data, size_t sz)
			{
				WriteLock<OS> lock(sz);
				lock.lock(m_os);
				mem::
				lock.begin()
				lock.unlock(m_os);
			}
		};
		template <typename IS>
		class XorIStream :public InStream<XorIStream<IS>, StreamInfo<typename IS::Component>>
		{
			static_assert(IsIStream<IS>::value, "IS is not InStream");

		private:
			IS * m_is;
			dword m_key;
			dword m_add;

		public:
			XorIStream(IS * is, dword key, dword add = 3508816465) noexcept
				: m_is(is), m_key(key), m_add(add)
			{
			}

			size_t readImpl(typename IS::Component * data, size_t sz)
			{
				return m_is->read(data, sz);
			}
		};
	}
}