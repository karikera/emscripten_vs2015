#pragma once

#include "ref.h"
#include "exports.h"

namespace kr
{
	class JsExternalData:public Interface<>
	{
		friend V8Class;
		friend _pri_::InternalTools;
	protected:
		int m_reference;
		v8::Persistent<v8::External> m_external;

		CBS_EASYV8_DLLEXPORT void _toWeak() noexcept;

	public:
		CBS_EASYV8_DLLEXPORT JsExternalData() noexcept;
		CBS_EASYV8_DLLEXPORT ~JsExternalData() noexcept;
		virtual void remove() noexcept = 0;

		inline void AddRef() noexcept
		{
			m_reference++;
		}
		inline void Release() noexcept
		{
			m_reference--;
			if (m_reference == 0)
				_toWeak();
		}
	};
}
