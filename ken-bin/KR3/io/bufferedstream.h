#pragma once

#include "filterstream.h"

namespace kr
{
	namespace io
	{
		template <typename Base, bool autoClose, size_t BUFFER_SIZE>
		class BufferedIStream 
			:public FilterIStream<BufferedIStream<Base, autoClose, BUFFER_SIZE>, Base, autoClose>
		{
		public:
			using Super = FilterIStream<BufferedIStream<Base, autoClose, BUFFER_SIZE>, Base, autoClose>;
			using Component = typename Base::Component;
			using m = memt<sizeof(Component)>;
			using Ref = RefArray<Component>;

			using Super::read;
			using Super::base;

			BufferedIStream(Base* p) noexcept 
				: Super(p)
			{
				m_filled = m_read = m_buffer;
			}
			BufferedIStream(nullptr_t) noexcept 
				: BufferedIStream((Base*)nullptr)
			{
				m_filled = m_read = m_buffer;
			}
			BufferedIStream(typename Base::BaseStream* p) noexcept
				: BufferedIStream(p->template retype<Component>())
			{
			}

			void resetStream(Base * p) noexcept
			{
				m_filled = m_read = m_buffer;
				Super::resetStream(p);
			}
			void resetStream(nullptr_t) noexcept
			{
				resetStream((Base*)nullptr);
			}
			void resetStream(typename Base::BaseStream* p) noexcept
			{
				resetStream(p->template retype<Component>());
			}


			Component peek()
			{
				for (;;)
				{
					if (m_filled > m_read)
					{
						return *m_read;
					}

					size_t sz = base()->read(m_buffer, BUFFER_SIZE);
					m_filled = m_buffer + sz;
					m_read = m_buffer;
				}
			}
			template <typename _Derived, typename _Info>
			size_t readLine(OutStream<_Derived, Component, _Info> * dest)
			{
				size_t readed = 0;
				Component * line;
				for (;;)
				{
					size_t remain = m_filled - m_read;
					line = m::find(m_read, (Component)'\n', remain);
					if (line != nullptr) break;
					if (remain != 0)
					{
						Component * last = m_filled;
						Component lastchr = last[-1];
						if (lastchr == (Component)'\r')
						{
							remain--;
							m_filled = m_buffer + 1;
							dest->write(m_read, remain);
							*m_buffer = lastchr;
						}
						else
						{
							m_filled = m_buffer;
							dest->write(m_read, remain);
						}
						readed += remain;
						m_read = m_buffer;
					}

					try
					{
						size_t res = base()->read(m_filled, m_buffer + BUFFER_SIZE - m_filled);
						m_filled += res;
					}
					catch (EofException&)
					{
						if(readed == 0) throw;
						return readed;
					}
				}

				size_t len = line - m_read;
				size_t copylen = len;
				if (line != m_read && line[-1] == '\r') copylen--;
				dest->write(m_read, copylen);
				m_read += len + 1;
				return readed + len;
			}
			size_t skipLine()
			{
				size_t readed = 0;
				Component * line;
				for (;;)
				{
					size_t remain = m_filled - m_read;
					line = m::find(m_read, (Component)'\n', remain);
					if (line != nullptr) break;
					if (remain != 0)
					{
						Component * last = m_filled;
						Component lastchr = last[-1];
						if (lastchr == (Component)'\r')
						{
							remain--;
							m_filled = m_buffer + 1;
							*m_buffer = lastchr;
						}
						else
						{
							m_filled = m_buffer;
						}
						readed += remain;
						m_read = m_buffer;
					}

					try
					{
						size_t res = base()->read(m_filled, m_buffer + BUFFER_SIZE - m_filled);
						m_filled += res;
					}
					catch (EofException&)
					{
						if (readed == 0) throw;
						return readed;
					}
				}

				size_t len = line - m_read;
				m_read += len + 1;
				return readed + len;
			}
			template <typename LAMBDA>
			size_t skipto_F(const LAMBDA &lambda)
			{
				size_t readlen = 0;
				Component * line;
				for (;;)
				{
					size_t size = m_filled - m_read;
					if (size != 0)
					{
						line = (Component*)lambda(RefArray<Component>(m_read, size));
						if (line != nullptr) break;
						Component * last = m_filled;
						readlen += size;
					}

					m_filled = m_read = m_buffer;
					m_filled += base()->read(m_filled, BUFFER_SIZE);
				}

				size_t copylen = line - m_read;
				m_read = line;
				return readlen + copylen;
			}
			template <typename _Derived, typename _Info, typename LAMBDA>
			size_t readto_F(OutStream<_Derived, Component, _Info> * dest, const LAMBDA &lambda)
			{
				size_t readed = 0;
				Component * line;
				for (;;)
				{
					Component * last = m_filled;
					size_t size = last - m_read;
					if (size != 0)
					{
						line = (Component*)lambda(RefArray<Component>(m_read, size));
						if (line != nullptr) break;
						dest->write(m_read, size);
						readed += size;
					}

					m_filled = m_read = m_buffer;

					try
					{
						m_filled += base()->read(m_buffer, BUFFER_SIZE);
					}
					catch (EofException&)
					{
						if (readed == 0) throw;
						return readed;
					}
				}

				size_t len = line - m_read;
				dest->write(m_read, len);
				m_read = line;
				return readed + len;
			}
			size_t skipto(Component chr)
			{
				return skipto_F([chr](Ref text) {
					return text.find(chr).begin();
				});
			}
			template <typename _Derived, typename _Info>
			size_t readto(OutStream<_Derived, Component, _Info> * dest, const Component &chr) // TooBigException
			{
				size_t sz = readto_F(dest, [chr](Ref text) {
					return text.find(chr).begin();
				});
				skip(1);
				return sz;
			}
			void skip(size_t sz)
			{
				Component * line;
				for (;;)
				{
					line = m_read + sz;
					if (line <= m_filled) break;
					sz = line - m_filled;
					size_t sz = base()->read(m_buffer, BUFFER_SIZE);
					m_filled = m_buffer + sz;
					m_read = m_buffer;
				}

				m_read = line;
			}
			size_t skipto_y(Ref chr)
			{
				return skipto_F([chr](Ref text) {
					return text.find_y(chr).begin();
				});
			}
			template <typename _Derived, typename _Info>
			size_t readto_y(OutStream<_Derived, Component, _Info> * dest, Ref chr)
			{
				return readto_F(dest, [chr](Ref text) {
					return text.find_y(chr).begin();
				});
			}
			size_t skipto_n(Component chr)
			{
				return skipto_F([chr](Ref text) {
					return text.find_n(chr).begin();
				});
			}
			template <typename _Derived, typename _Info>
			size_t readto_n(OutStream<_Derived, Component, _Info> * dest, Component chr)
			{
				return readto_F(dest, [chr](Ref text) {
					return text.find_n(chr).begin();
				});
			}
			size_t skipto_ny(Ref chr)
			{
				return skipto_F([chr](Ref text) {
					return text.find_ny(chr).begin();
				});
			}
			template <typename _Derived, typename _Info>
			size_t readto_ny(OutStream<_Derived, Component, _Info> * dest, Ref chr)
			{
				return readto_F(dest, [chr](Ref text) {
					return text.find_ny(chr).begin();
				});
			}
			template <typename _Derived, typename _Info>
			int readto_unslash_linecount(OutStream<_Derived, Component, _Info> * dest, Component needle)
			{
				int line = 0;
				for (;;)
				{
					readto_F(dest, [&](Ref text) {
						const Component needles[] = { (Component)'\\', needle };
						Ref finded = text.find_y(Ref(needles, countof(needles)));
						if (finded != nullptr)
							line += (int)text.cut(finded).count('\n');
						return finded.begin();
					});
					Component findedchr = read();

					if (findedchr == '\\')
					{
						Component chr = read();
						switch (chr)
						{
						case 'a': dest->write('\a'); break;
						case 'b': dest->write('\b'); break;
						case 'f': dest->write('\f'); break;
						case 'r': dest->write('\r'); break;
						case 'n': dest->write('\n'); break;
						case 't': dest->write('\t'); break;
						case '\"':
						case '\'':
						case '\\':
						default:
							dest->write(chr);
							break;
						}
					}
					else return line;
				}
			}
			void skipto_unslash(Component needle)
			{
				for (;;)
				{
					skipto_F([&](Ref text) {
						const Component needles[] = { (Component)'\\', needle };
						Ref finded = text.find_y(Ref(needles, countof(needles)));
						return finded.begin();
					});

					if (read() == '\\')
					{
						skip(1);
					}
					else return;
				}
			}
			template <typename _Derived, typename _Info>
			void readto_unslash(OutStream<_Derived, Component, _Info> * dest, Component needle)
			{
				for (;;)
				{
					readto_F(dest, [&](Ref text) {
						const Component needles[] = { (Component)'\\', needle };
						Ref finded = text.find_y(Ref(needles, countof(needles)));
						return finded.begin();
					});
					Component findedchr = read();

					if (findedchr == '\\')
					{
						Component chr = read();
						switch (chr)
						{
						case 'a': dest->write('\a'); break;
						case 'b': dest->write('\b'); break;
						case 'f': dest->write('\f'); break;
						case 'r': dest->write('\r'); break;
						case 'n': dest->write('\n'); break;
						case 't': dest->write('\t'); break;
						case '\"':
						case '\'':
						case '\\':
						default:
							dest->write(chr);
							break;
						}
					}
					else return;
				}
			}

			RefArray<Component> getBuffer() noexcept
			{
				return RefArray<Component>(m_read, m_filled);
			}
			size_t readImpl(Component * data, size_t sz)
			{
				Component* dest = data;
				Component* destto = dest + sz;

				try
				{
					for (;;)
					{
						{
							size_t need = destto - dest;
							Component* readto = m_read + need;
							if (m_filled >= readto)
							{
								mema::copy(dest, m_read, need);
								m_read = readto;
								dest = destto;
								break;
							}
						}

						size_t left = m_filled - m_read;
						mema::copy(dest, m_read, left);
						dest += left;
						m_read = m_buffer;

						size_t res = base()->read(m_buffer, BUFFER_SIZE);
						m_filled = m_read + res;
					}
				}
				catch (EofException&)
				{
					if (dest == data) throw;
				}
				return dest - data;
			}

		private:
			Component m_buffer[BUFFER_SIZE];
			Component* m_read;
			Component* m_filled;
		};
		
		template <typename Base, bool endFlush, bool autoClose, size_t BUFFER_SIZE>
		class BufferedOStream 
			:public FilterOStream<BufferedOStream<Base, endFlush, autoClose, BUFFER_SIZE>, Base, autoClose>
		{
			CLASS_HEADER(BufferedOStream, FilterOStream<BufferedOStream<Base, endFlush, autoClose, BUFFER_SIZE>, Base, autoClose>);
		public:
			INHERIT_COMPONENT();

			using Super::base;

			BufferedOStream(Base* p) noexcept 
				: Super(p)
			{
				m_filled = m_buffer;
			}
			BufferedOStream(nullptr_t) noexcept
				: BufferedOStream((Base*)nullptr)
			{
			}
			BufferedOStream(typename Base::BaseStream * p) noexcept 
				: BufferedOStream(p->template retype<Component>())
			{
			}
			~BufferedOStream() noexcept
			{
				if (endFlush)
					flush();
			}

			void resetStream(nullptr_t) noexcept
			{
				m_filled = m_buffer;
				Super::resetStream(nullptr);
			}
			void resetStream(Base * p) noexcept
			{
				m_filled = m_buffer;
				Super::resetStream(p);
			}
			void resetStream(typename Base::BaseStream * p) noexcept
			{
				m_filled = m_buffer;
				Super::resetStream(p);
			}
			void writeImpl(const Component* data, size_t sz)
			{
				const InternalComponent* src = (InternalComponent*)data;
				size_t left = m_buffer + BUFFER_SIZE - m_filled;
				if (left > sz)
				{
					mema::copy(m_filled, src, sz);
					m_filled += sz;
					return;
				}

				const InternalComponent* srcto = src + sz;
				mema::copy(m_filled, src, left);
				src += left;

				base()->write(m_buffer, BUFFER_SIZE);
				while (src + BUFFER_SIZE <= srcto)
				{
					base()->write(src, BUFFER_SIZE);
					src += BUFFER_SIZE;
				}
				left = srcto - src;
				mema::copy(m_buffer, src, left);
				m_filled = m_buffer + left;
				src = srcto;
			}
			void flush()
			{
				InternalComponent* ptr = (InternalComponent*)m_buffer;
				base()->write((Component*)m_buffer, m_filled - ptr);
				m_filled = m_buffer;
			}

		private:
			InternalComponent m_buffer[BUFFER_SIZE];
			InternalComponent* m_filled;
		};

	}
}
