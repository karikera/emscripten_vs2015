#pragma once

namespace kr
{
	
	class autoptr final
	{
	public:
		autoptr() noexcept;
		autoptr(nullptr_t) noexcept;
		autoptr& operator =(nullptr_t) noexcept;
		template <typename T> autoptr(T* dat) noexcept;
		template <typename T> operator T*() const noexcept;
		template <typename T> autoptr& operator =(T* dat) noexcept;

	protected:
		ptr m_ptr;
	};

	template <typename T>
	inline autoptr::autoptr(T * dat) noexcept
	{
		*this = dat;
	}
	template <typename T>
	inline autoptr::operator T*() const noexcept
	{
		return (T*)m_ptr;
	}
	template <typename T>
	inline autoptr& autoptr::operator =(T* dat) noexcept
	{
		m_ptr = (ptr)dat;
		return *this;
	}
}