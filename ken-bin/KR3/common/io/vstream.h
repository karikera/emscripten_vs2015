#pragma once

#include <KR3/main.h>
#include "istream.h"
#include "ostream.h"

namespace kr
{
	namespace io
	{
		template <typename C, class Parent = Empty> class VIStream;
		template <typename C, class Parent = Empty> class VOStream;


		template <typename C, class Parent>
		class VIStream : public InStream<VIStream<C, Parent>, C, StreamInfo<false, Parent>>
		{
		private:
			void * m_stream;
			size_t(*m_read)(void * stream, C * data, size_t sz);

		public:
			VIStream() noexcept
			{
				m_read = [](void * stream, C * data, size_t sz)->size_t { throw EofException(); };
			}
			template <class S> VIStream(S * stream) noexcept
				: m_stream(stream)
			{
				static_assert(IsIStream<S>::value, "is not InStream");
				m_read = [](void * stream, C * data, size_t sz)->size_t { return ((S*)stream)->read(data, sz); };
			}

			size_t readImpl(C* data, size_t sz)
			{
				return m_read(m_stream, data, sz);
			}
		};
		template <typename C, class Parent>
		class VOStream : public OutStream<VOStream<C, Parent>, C, StreamInfo<false, Parent>>
		{
		private:
			void * m_stream;
			void(*m_write)(void * stream, const C * data, size_t sz);

		public:
			template <class S> VOStream(S * stream) noexcept
				: m_stream(stream)
			{
				static_assert(IsOStream<S>::value, "S is not OutStream");
				m_write = [](void * stream, const C * data, size_t sz) { ((S*)stream)->write(data, sz); };
			}

			void writeImpl(const C* data, size_t sz)
			{
				return m_write(m_stream, data, sz);
			}
		};

		template <typename C, class Parent>
		class VIOStream : public InStream<VIOStream<C, Parent>, C, StreamInfo<false, OutStream<VIOStream<C, Parent>, C, StreamInfo<false, Parent>>>>
		{
		private:
			void * m_stream;
			void(* m_write)(void * stream, const C * data, size_t sz);
			size_t(* m_read)(void * stream, C * data, size_t sz);

		public:
			template <class S> VIOStream(S * stream) noexcept
				: m_stream(stream)
			{
				static_assert(IsIStream<S>::value, "S is not InStream");
				m_write = [](void * stream, const C * data, size_t sz) { ((S*)stream)->write(data, sz); };
				m_read = [](void * stream, C * data, size_t sz)->size_t { return ((S*)stream)->read(data, sz); };
			}

			void writeImpl(const C* data, size_t sz)
			{
				return m_write(m_stream, data, sz);
			}
			size_t readImpl(C* data, size_t sz)
			{
				return m_read(m_stream, data, sz);
			}
		};
	}
}