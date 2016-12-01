#pragma once

#include <KR3/main.h>

namespace kr
{
	template <typename T>
	class EnviromentVariableT: public Bufferable<EnviromentVariableT<T>, BufferInfo<T, false, true, true, Empty>>
	{
	public:
		EnviromentVariableT(const T* szname) noexcept;
		bool set(const T * value) noexcept;
		size_t copyTo(T * dest) const noexcept;
		size_t size() const noexcept;
		bool exists() const noexcept;

	private:
		const T * m_name;
		dword m_size;
	};

	template <>
	EnviromentVariableT<char>::EnviromentVariableT(const char * szname) noexcept;
	template <>
	bool EnviromentVariableT<char>::set(const char * dest) noexcept;
	template <>
	size_t EnviromentVariableT<char>::copyTo(char * dest) const noexcept;
	
	template <typename T>
	size_t EnviromentVariableT<T>::size() const  noexcept
	{
		return m_size;
	}
	template <typename T>
	bool EnviromentVariableT<T>::exists() const  noexcept
	{
		return m_size != 0;
	}

	using EnviromentVariable = EnviromentVariableT<char>;

#ifdef WIN32
	template <>
	EnviromentVariableT<wchar>::EnviromentVariableT(const wchar * szname) noexcept;
	template <>
	bool EnviromentVariableT<wchar>::set(const wchar * dest) noexcept;
	template <>
	size_t EnviromentVariableT<wchar>::copyTo(wchar * dest) const noexcept;
	using EnviromentVariableW = EnviromentVariableT<wchar>;
#endif
}