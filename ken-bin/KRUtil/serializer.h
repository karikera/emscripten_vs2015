#pragma once

#include <KR3/main.h>
#include <vector>

namespace kr
{
	template <typename OS> class Serializer;
	template <typename IS> class Deserializer;

	namespace _pri_
	{
		template <typename T, bool is_class> struct BaseSerialize;
		template <typename T> struct BaseSerialize<T, true>
		{
			template <typename S>
			inline static void serialize(T& dest, S & serializer)
			{
				dest.serialize(serializer);
			}
		};
		template <typename T> struct BaseSerialize<T, false>
		{
			template <typename S>
			inline static void serialize(T& dest, S & serializer)
			{
				serializer.raw(dest);
			}
		};
	}
	template <typename T> 
	struct Serialize : _pri_::BaseSerialize<T, std::is_class<T>::value>
	{
	};
	template <typename C>
	struct Serialize<std::vector<C>>
	{
		template <typename S>
		inline static void serialize(std::vector<C>& dest, S & s)
		{
			s.serializeSize(&dest);
			for (C & c: dest)
				s & c;
		}
	};
	template <typename C>
	struct Serialize<std::basic_string<C>>
	{
		template <typename S>
		inline static void serialize(std::basic_string<C>& dest, S & s)
		{
			s.serializeSize(&dest);
			for (C & c : dest)
				s & c;
		}
	};
	template <typename C, size_t size>
	struct Serialize<C[size]>
	{
		template <typename S>
		inline static void serialize(C(&dest)[size], S & serializer)
		{
			for (C & c : dest)
			{
				serializer & dest;
			}
		}
	};

	class SerializedSizer
	{
	private:
		size_t m_size;

	public:
		static constexpr bool read = true;
		static constexpr bool write = false;
		inline SerializedSizer() noexcept
		{
			m_size = 0;
		}
		template <typename T>
		inline void raw(T &) noexcept
		{
			m_size += sizeof(T);
		}
		template <typename T>
		inline void serializeSize(T * buffer)
		{
			size_t sz = buffer->size();
			if(sz == 0) m_size ++;
			else m_size += math::plog2(sz) / 7 + 1;
		}
		template <typename T>
		inline SerializedSizer& operator &(T & t)
		{
			Serialize<T>::serialize(t, *this);
			return *this;
		}
		template <typename T>
		inline SerializedSizer& operator <<(T & t)
		{
			Serialize<T>::serialize(t, *this);
			return *this;
		}
		template <typename T>
		inline SerializedSizer& operator >>(T & t)
		{
			Serialize<T>::serialize(t, *this);
			return *this;
		}
		size_t size() const noexcept
		{
			return m_size;
		}
		template <typename T>
		static size_t getSize(const T & value)
		{
			SerializedSizer sizer;
			sizer & const_cast<T&>(value);
			return sizer.m_size;
		}
	};

	template <class OS> class Serializer
	{
		static_assert(is_same<typename OS::Component, void>::value, "Component must be void");
	private:
		OS * const m_os;

	public:
		static constexpr bool read = false;
		static constexpr bool write = true;

		inline Serializer(OS * os) noexcept
			:m_os(os)
		{
		}
		template <typename T>
		inline void raw(T & t)
		{
			m_os->write((void*)&t, sizeof(T));
		}
		inline void leb128(size_t value)
		{
			byte result;
			for (;;)
			{
				result = value & 0x7f;
				value >>= 7;
				if (value == 0)
					break;
				result |= 0x80;
				m_os->write(&result, 1);
			}
			m_os->write(&result, 1);
		}
		inline void kr_leb128(size_t value)
		{
			byte out;
			while (value >= 0x80)
			{
				out = (byte)(value & 0x7f);
				m_os->write(&out, 1);
				value >>= 7;
			}
			out = (byte)(value | 0x80);
			m_os->write(&out, 1);
		}
		template <typename C>
		inline void reference(const C* ref, size_t sz)
		{
			m_os->write(ref, sz * sizeof(C));
		}
		template <typename C>
		inline void copy(const C* p, size_t sz)
		{
			if (m_os->write(p, sz) != sz)
				throw NotEnoughSpaceException();
		}
		template <typename T>
		inline void serializeSize(T * buffer)
		{
			kr_leb128(buffer->size());
		}
		template <typename T>
		inline Serializer& operator &(const T & t)
		{
			Serialize<T>::serialize((T&)t, *this);
			return *this;
		}
		template <typename T>
		inline Serializer& operator <<(const T & t)
		{
			Serialize<T>::serialize((T&)t, *this);
			return *this;
		}
	};
	template <class IS> class Deserializer
	{
		static_assert(is_same<typename IS::Component, void>::value, "Component must be void");
	private:
		IS * const m_is;

	public:
		static constexpr bool read = true;
		static constexpr bool write = false;

		inline Deserializer(IS * is) noexcept
			: m_is(is)
		{
		}
		template <typename T>
		inline void raw(T & t)
		{
			m_is->read((void*)&t, sizeof(T));
		}
		inline void leb128(size_t &value) noexcept
		{
			dword result = 0;
			dword shift = 0;
			while (true)
			{
				byte v;
				m_is->read(&v, 1);
				result |= (v & 0x7f) << shift;
				if ((v & 0x80) == 0)
					break;
				shift += 7;
			}
			value = result;
		}
		inline void kr_leb128(size_t & sz)
		{
			size_t v = 0;
			byte shift = 0;
			for (;;)
			{
				byte d;
				m_is->read(&d, 1);
				if ((d & 0x80) != 0)
				{
					v |= (d & 0x7f) << shift;
					break;
				}
				v |= d << shift;
				shift += 7;
			}
			sz = v;
		}
		template <typename C>
		inline void reference(const C* &ref, size_t sz)
		{
			ref = (C*)m_is->begin();
			m_is->addBegin(sz * sizeof(C));
		}
		template <typename C>
		inline void copy(C* p, size_t sz)
		{
			if (m_is->read(p, sz) != sz)
				throw EofException();
		}
		template <typename T>
		inline void serializeSize(T * buffer)
		{
			size_t sz;
			kr_leb128(sz);
			buffer->resize(sz);
		}
		template <typename T>
		inline Deserializer& operator &(T & t)
		{
			Serialize<T>::serialize(t, *this);
			return *this;
		}
		template <typename T>
		inline Deserializer& operator >>(T & t)
		{
			Serialize<T>::serialize(t, *this);
			return *this;
		}
	};
}