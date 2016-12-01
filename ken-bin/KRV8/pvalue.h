#pragma once

#include <assert.h>
#include <utility>

namespace kr
{
	template <typename T, typename MASTER>
	class ProtectValue
	{
		friend MASTER;
	protected:
		T m_value;

		inline ProtectValue(const ProtectValue &value)
			: m_value(value)
		{
		}
		inline ProtectValue(ProtectValue &&value)
			: m_value(move(value))
		{
		}
		inline ProtectValue(const T &value)
			: m_value(value)
		{
		}
		inline ProtectValue(T &&value)
			: m_value(move(value))
		{
		}
		inline decltype(&*(T*)0) operator &()
		{
			return &m_value;
		}
		inline ProtectValue& operator =(const ProtectValue& pv)
		{
			m_value = pv.m_value;
			return *this;
		}
		inline ProtectValue& operator =(ProtectValue&& pv)
		{
			m_value = move(pv.m_value);
			return *this;
		}
		inline ProtectValue& operator =(const T& value)
		{
			m_value = value;
			return *this;
		}
		inline ProtectValue& operator =(T&& value)
		{
			m_value = move(value);
			return *this;
		}

	public:
		inline ProtectValue()
			:m_value()
		{
		}
		inline operator const T&() const
		{
			return m_value;
		}
		const T& operator ->() const
		{
			assert(m_value != nullptr);
			return m_value;
		}

	};
}