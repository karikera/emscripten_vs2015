#pragma once

#include <KR3/main.h>

namespace kr
{
	namespace _pri_
	{

		template <typename OS, typename IS, bool os_buffered, bool is_buffered, size_t BSIZE> class Pipe
		{
		public:
			static constexpr size_t BUFFER_SIZE = BSIZE != -1 ? BSIZE : 8192;
			static_assert(is_same<typename IS::Component, typename OS::Component>::value, "Is not same type");
			using Component = typename OS::Component;

			Pipe(OS * os, IS * is) noexcept
				:m_buffer(_new byte[BUFFER_SIZE])
			{
				m_is = is;
				m_os = os;
				m_readp = m_writep = (Component*)(byte*)m_buffer;
			}

			void streaming()
			{
				if (m_writep == m_readp)
				{
					m_readp = m_writep = (Component*)(byte*)m_buffer;
					size_t readed = m_is->read(m_writep, BUFFER_SIZE / sizeof(Component));
					m_writep += readed;
				}
				m_os->write(m_readp, m_writep - m_readp);
				m_readp = m_writep;
			}

		private:
			IS* m_is;
			OS* m_os;
			Must<byte> m_buffer;
			Component * m_writep;
			Component * m_readp;

		};

		template <typename OS, typename IS, bool os_buffered>
		class Pipe<OS, IS, os_buffered, true, -1>
		{
		public:
			static_assert(is_same<typename IS::Component, typename OS::Component>::value, "Is not same type");
			using Component = typename OS::Component;

			Pipe(OS * os, IS * is) noexcept
			{
				m_is = is;
				m_os = os;
			}
			bool streaming()
			{
				const Component * src = m_is->read(&size);
				m_os->write(src, size);
			}
		private:
			IS* m_is;
			OS* m_os;
		};
		template <typename OS, typename IS>
		class Pipe<OS, IS, true, false, -1>
		{
		public:
			static_assert(is_same<typename IS::Component, typename OS::Component>::value, "Is not same type");
			using Component = typename OS::Component;
			static constexpr size_t MINIMUM_BUFFER = 2048;

			Pipe(OS * os, IS * is) noexcept
			{
				m_is = is;
				m_os = os;
			}
			bool streaming()
			{
				if (m_os->left() < MINIMUM_BUFFER) m_os->padding(MINIMUM_BUFFER);
				size_t sz = m_is->read(m_os->end(), m_os->left());
				m_os->commit(sz);
				return true;
			}
		private:
			IS* m_is;
			OS* m_os;
		};
	}
	template <typename OS, typename IS, size_t BSIZE = -1> class Pipe
		: public _pri_::Pipe<OS, IS, OS::accessable, IS::accessable, BSIZE>
	{
		static_assert(IsIStream<IS>::value, "IS is not InStream");
		static_assert(IsOStream<OS>::value, "OS is not OutStream");
		CLASS_HEADER(Pipe, _pri_::Pipe<OS, IS, OS::accessable, IS::accessable, BSIZE>);
	public:
		using Super::Super;
	};

	template <typename OS, typename IS> void streaming(OS * os, IS * is)
	{
		try
		{
			Pipe<OS, IS> pipe(os, is);
			for(;;) pipe.streaming();
		}
		catch (EofException&)
		{
		}
	}
}