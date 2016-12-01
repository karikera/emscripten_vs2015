#pragma once

#include <KR3/main.h>

#include "container.h"
#include "io/lock.h"
#include "io/stream.h"
#include "text/buffer.h"
#include "text/method/buffer_i.h"
#include "text/method/buffer_io.h"
#include "text/method/stream.h"
#include "text/method/utils.h"
#include "text/wrap.h"
#include "text/numformat.h"
#include "text/nullterm.h"
#include "text/join.h"
#include "text/encoder.h"

namespace kr
{
	template <typename C>
	class TempSzText:public TmpArray<C>
	{
		CLASS_HEADER(TempSzText, TmpArray<C>);
	public:
		INHERIT_COMPONENT();

		using Super::c_str;

		TempSzText() noexcept
			:Super((size_t)0, 1024)
		{
		}
		TempSzText(size_t size) noexcept
			: Super(size)
		{
		}
		TempSzText(size_t size, size_t capacity) noexcept
			: Super(size, capacity)
		{
		}
		TempSzText(TempSzText&& _move) noexcept
			:Super((Super&&)_move)
		{
		}
		TempSzText(Super&& _move) noexcept
			: Super(move(_move))
		{
		}
		template <typename ARG1, typename ... ARGS>
		explicit TempSzText(const ARG1 & arg1, const ARGS & ... args) noexcept
		{
			prints(arg1, args ...);
		}
		TempSzText& operator = (const TempSzText &) noexcept = delete;

		template <typename T> TempSzText& operator <<(const T& temp)
		{
			*(Super*)this << temp;
			return *this;
		}
		operator C*() noexcept
		{
			return c_str();
		}
	};

	inline kr::Text operator ""_tx(const char * tx, size_t sz)
	{
		return kr::Text(tx, sz);
	}
	inline kr::Text16 operator ""_tx(const char16 * tx, size_t sz)
	{
		return kr::Text16(tx, sz);
	}
	inline kr::Text32 operator ""_tx(const char32 * tx, size_t sz)
	{
		return kr::Text32(tx, sz);
	}
	inline kr::TextW operator ""_tx(const wchar * tx, size_t sz)
	{
		return kr::TextW(tx, sz);
	}
}

template <typename C>
struct std::hash<kr::RefArray<C>>
{
	size_t operator ()(const kr::RefArray<C> & buffer) const noexcept
	{
		return kr::mem::hash(buffer.begin(), buffer.sizeBytes());
	}
};
template <typename C>
struct std::hash<kr::Array<C>>
{
	size_t operator ()(const kr::RefArray<C> & buffer) const noexcept
	{
		return kr::mem::hash(buffer.begin(), buffer.sizeBytes());
	}
};
template <typename C, size_t cap>
struct std::hash<kr::BArray<C, cap>>
{
	size_t operator ()(const kr::BArray<C, cap> & buffer) const noexcept
	{
		return kr::mem::hash(buffer.begin(), buffer.sizeBytes());
	}
};
