#pragma once

#include "../main.h"
#include "../meta/log2.h"

namespace kr
{

	template <typename Derived>
	class BinArrayMethod
	{
	private:
		uintp * _data() noexcept
		{
			return static_cast<Derived*>(this)->data();
		}
		size_t _sizep() noexcept
		{
			return static_cast<Derived*>(this)->sizep();
		}
		size_t _size() noexcept
		{
			return static_cast<Derived*>(this)->size();
		}

	public:
		static constexpr size_t bitsize = sizeof(uintp) * 8;
		static constexpr size_t bitmask = bitsize - 1;
		static constexpr size_t bitshift = meta::ilog2(bitsize);

		bool isAllDisabled() noexcept
		{
			return memd::filled(_data(), 0, (_size() + bitsize - 1) >> bitshift);
		}
		void enable(size_t n) noexcept
		{
			_assert(n < _size());
			_data()[n >> bitshift] |= ((size_t)1 << (n & bitmask));
		}
		void disable(size_t n) noexcept
		{
			_assert(n < _size());
			_data()[n >> bitshift] &= ~((size_t)1 << (n & bitmask));
		}
		void toggle(size_t n) noexcept
		{
			_assert(n < _size());
			_data()[n >> bitshift] ^= ((size_t)1 << (n & bitmask));
		}
		void enableRange(size_t n, size_t len) noexcept
		{
			uintp* p = _data() + (n >> bitshift);
			uintp left = n & bitmask;
			if (left != 0)
			{
				len += left - bitsize;
				uintp v = 0;
				while (left < bitsize)
				{
					v |= ((uintp)1 << left);
					left++;
				}
				*p |= v;
				p++;
			}

			uintp* end = p + (len >> bitshift);
			memset(p, 0xff, (pbyte)end - (pbyte)p);

			left = len & bitmask;
			if (left != 0)
			{
				uintp v = 0;
				for (size_t i = 0; i<left; i++)
					v |= ((uintp)1 << i);
				*end |= v;
			}
		}
		void disableRange(size_t n, size_t len) noexcept
		{
			uintp* p = _data() + (n >> bitshift);
			uintp left = n & bitmask;
			if (left != 0)
			{
				len += left - bitsize;
				uintp v = 0;
				while (left < bitsize)
				{
					v |= (1 << left);
					left++;
				}
				*p &= ~v;
				p++;
			}

			uintp* end = p + (len >> bitshift);
			memset(p, 0, (pbyte)end - (pbyte)p);

			left = len & bitmask;
			if (left != 0)
			{
				uintp v = 0;
				for (size_t i = 0; i<left; i++)
					v |= (1 << i);
				*end &= ~v;
			}
		}
		void enableAll() noexcept
		{
			mem::set(_data(), 0xff, _sizep() * sizeof(uintp));
		}
		void disableAll() noexcept
		{
			mem::zero(_data(), _sizep() * sizeof(uintp));
		}
		void set(size_t n, bool v) noexcept
		{
			_assert(n < _size());
			puintp pmap = (_data() + (n >> 5));
			n = v << (n & bitmask);
			*pmap = (*pmap & ~n) | n;
		}
		bool get(size_t n) noexcept
		{
			_assert(n < _size());
			return ((_data()[n >> bitshift]) & ((size_t)1 << (n & bitmask))) != 0;
		}
		size_t findEnabled(size_t index) noexcept
		{
			uintp * data = _data();
			uintp * end = data + _sizep();
			uintp * p = data + (index >> bitshift);
			if (p >= end) return -1;

			uint left = index  & bitmask;
			if (left > 0)
			{
				uintp v = *p;
				while (left < bitsize)
				{
					if (v & ((uintp)1 << left))
						return index;
					left++;
					index++;
				}
				p++;
			}

			for (; p < end; p++)
			{
				if (*p == 0) continue;

				left = 0;
				uintp v = *p;
				while (left < bitsize)
				{
					if (v & ((uintp)1 << left))
						return (p - data)*bitsize + left;
					left++;
				}
			}
			return -1;
		}
		const bool operator [](size_t n) noexcept
		{
			return get(n);
		}
	};
	
	template <size_t SIZE>
	class BBinArray : public BinArrayMethod<BBinArray<SIZE> >
	{
		using BinArrayMethod<BBinArray<SIZE> >::bitsize;
		static constexpr size_t _sizep = (SIZE + bitsize - 1) / bitsize;
	public:
		uintp * data() noexcept
		{
			return m_map;
		}
		size_t size() noexcept
		{
			return SIZE;
		}
		size_t sizep() noexcept
		{
			return _sizep;
		}

	private:
		uintp m_map[_sizep];
	};

	class BinArray : public BinArrayMethod<BinArray>
	{
	public:
		BinArray() noexcept;
		BinArray(size_t size) noexcept;
		~BinArray() noexcept;
		void alloc(size_t size) noexcept;
		uintp * data() noexcept;
		size_t size() noexcept;
		size_t sizep() noexcept;
		void resizeAsFalse(size_t to) noexcept;
		void free() noexcept;

	private:
		static constexpr size_t bitsize = sizeof(uintp) * 8;
		puintp m_map;

	};
}