#pragma once

#include "../main.h"

namespace kr
{
	
	class Protocol
	{
	public:
		using GENERATOR = kr::Protocol* (*)(kr::Text uri);
		using GENERATOR16 = kr::Protocol* (*)(kr::TextW uri);

		Protocol();
		virtual ~Protocol();
		virtual size_t readFirst(ptr data, dword nSize);
		virtual size_t readNext(ptr data, dword nSize) =0;

		static Protocol * connect(TextW uri);
		static void regist(Text name, GENERATOR gen, TextW name16, GENERATOR16 gen16) noexcept;
		template <typename T> static void regist(Text name,TextW name16) noexcept
		{
			regist(name, &generator<T>, name16, &generator16<T>);
		}

	private:
		template <typename T> Protocol* generator(Text url)
		{
			return _new T(url);
		}
		template <typename T> Protocol* generator16(TextW url)
		{
			return _new T(url);
		}
	};


}