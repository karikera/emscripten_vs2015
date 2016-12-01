#pragma once

#include "../main.h"

namespace kr
{
	
	class Random
	{
	public:
		Random() noexcept;
		Random(qword seed) noexcept;
		dword getDword() noexcept;
		float getSignedFloat() noexcept;
		double getSignedDouble() noexcept;

		void fill(void * dest, size_t size) noexcept;
		template <size_t size> void fill(void * value) noexcept;

		template <typename T> T get() noexcept;
		template <typename T> T getSigned(T value) noexcept;
		template <typename T> T get(T value) noexcept;

	private:
		dword _getExp8() noexcept;
		dword _getExp11() noexcept;

		dword m_state[16];
		int m_index;
	};

	extern Random g_random;
}

template <> void kr::Random::fill<0>(void * value) noexcept;
template <> void kr::Random::fill<1>(void * value) noexcept;
template <> void kr::Random::fill<2>(void * value) noexcept;
template <> void kr::Random::fill<3>(void * value) noexcept;
template <> void kr::Random::fill<4>(void * value) noexcept;
template <> bool kr::Random::get<bool>() noexcept;
template <> kr::llong kr::Random::get<kr::llong>() noexcept;
template <> kr::qword kr::Random::get<kr::qword>() noexcept;
template <> float kr::Random::get<float>() noexcept;
template <> double kr::Random::get<double>() noexcept;
template <> float kr::Random::get<float>(float value) noexcept;
template <> double kr::Random::get<double>(double value) noexcept;

#include "random.inl"
