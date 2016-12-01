#pragma once

#include <ctime>
#include <KR3/main.h>

namespace kr
{
	using filetime_t = unsigned long long;

	namespace _pri_
	{
		
		template <typename C> struct DATETEXT
		{
			static RefArray<C> wkday[];
			static RefArray<C> month[];
		};

	}

	class TimeInformation :public Printable<TimeInformation, AutoComponent, std::tm>
	{
	public:

		template <class _Derived, typename C, class _Info>
		void writeTo(OutStream<_Derived, C, _Info> *str) const // NotEnoughSpaceException
		{
			// RFC1123 "wkd, DD MMM YYYY hh:mm:ss GMT"
			*str << _pri_::DATETEXT<C>::wkday[tm_wday] << (C)',' << (C)' '
				<< decf(tm_mday, 2) << (C)' '
				<< _pri_::DATETEXT<C>::month[tm_mon] << (C)' '
				<< decf(tm_year + 1900, 4) << (C)' '
				<< decf(tm_hour, 2) << (C)':'
				<< decf(tm_min, 2) << (C)':'
				<< decf(tm_sec, 2) << (C)' ' << (C)'G' << (C)'M' << (C)'T';
		}

		TimeInformation & operator = (const std::tm & t) noexcept
		{
			*static_cast<std::tm*>(this) = t;
			return *this;
		}

	};

	class UnixTimeStamp
	{
	public:
		UnixTimeStamp() noexcept;
		UnixTimeStamp(time_t uts) noexcept;
		UnixTimeStamp(filetime_t filetime) noexcept;
		UnixTimeStamp(Text strGMT); // OutOfRangeException, InvalidSourceException
		UnixTimeStamp(TextW strGMT); // OutOfRangeException, InvalidSourceException
		time_t getUTS() noexcept;
		filetime_t getFileTime() noexcept;
		UnixTimeStamp & operator =(time_t uts) noexcept;
		UnixTimeStamp & operator =(Text strGMT); // OutOfRangeException, InvalidSourceException
		UnixTimeStamp & operator =(TextW strGMT); // OutOfRangeException, InvalidSourceException

		bool operator <= (UnixTimeStamp uts) const noexcept;
		bool operator >= (UnixTimeStamp uts) const noexcept;
		bool operator < (UnixTimeStamp uts) const noexcept;
		bool operator > (UnixTimeStamp uts) const noexcept;
		bool operator == (UnixTimeStamp uts) const noexcept;
		bool operator != (UnixTimeStamp uts) const noexcept;

		TimeInformation getInfo() const noexcept;

		static struct TIMEZONE
		{
			char cSign;
			byte nHour;
			byte nMin;

			TIMEZONE() noexcept;
		} timeZone;

		static UnixTimeStamp now() noexcept;
		
	private:
		time_t m_uts;
		
		
		template <typename C> struct TimeMethods;

	};

}