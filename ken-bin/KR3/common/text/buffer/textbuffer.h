#pragma once

#include "../container.h"
#include "membuffer.h"
#include <string>

namespace kr
{
	namespace buffer
	{
		namespace _pri_
		{
			template <typename Component>
			struct StaticText
			{
				static const Component WHITE_SPACE[5];
			};
			template <typename Component>
			const Component StaticText<Component>::WHITE_SPACE[5] = {
				(Component)' ',
				(Component)'\t',
				(Component)'\r',
				(Component)'\n',
				(Component)'\0',
			};
		}

		template <class Derived, class Info, class Next>
		class TextBuffer : public Next
		{
			CLASS_HEADER(TextBuffer, Next);
		public:
			INHERIT_COMPONENT();

			using Super::Super;
			using Super::begin;
			using Super::end;
			using Super::size;

			static const typename Info::Component (&WHITE_SPACE)[5];

			friend std::basic_ostream<Component>& operator <<(std::basic_ostream<Component>& os, const TextBuffer& method)
			{
				os.write(method.begin(),method.size());
				return os;
			}

			qword to_qword_x(uint _radix, size_t _len) const noexcept
			{
				return memm::template toint<qword>(begin(),_len,_radix);
			}
			llong to_llong_x(uint _radix, size_t _len) const noexcept
			{
				return memm::template toint<llong>(begin(), _len, _radix);
			}
			dword to_uint_x(uint _radix, size_t _len) const noexcept
			{
				return memm::template toint<dword>(begin(),_len,_radix);
			}
			long to_int_x(uint _radix, size_t _len) const noexcept
			{
				return memm::template toint<long>(begin(), _len, _radix);
			}
			uintp to_uintp_x(uint _radix, size_t _len) const noexcept
			{
				return memm::template toint<uintp>(begin(), _len, _radix);
			}
			intp to_intp_x(uint _radix, size_t _len) const noexcept
			{
				return memm::template toint<intp>(begin(), _len, _radix);
			}
			float to_ufloat_x(size_t _len) const noexcept
			{
				const ComponentRef * beg = begin();
				const ComponentRef * finded = memm::find(beg, '.', _len);
				if (finded == nullptr) return (float)to_uint_x(10, _len);
				size_t len2 = finded - beg;
				_len = _len - len2 - 1;
				float res = (float)to_uint_x(10, len2);
				res += (float)(memm::template toint<dword>(finded + 1, _len, 10)) / math::pow((size_t)10, _len);
				return res;
			}
			float to_float_x(size_t _len) const noexcept
			{
				if (!_len) return 0;
				if (*begin() == (Component)'-')
					return -((*this + 1).to_ufloat_x(_len - 1));
				else return to_ufloat_x(_len);
			}
			bool numberonly_x(size_t _len) const noexcept
			{
				return (memm::numberonly(begin(), _len));
			}

			qword to_qword_limit_x(uint _radix, size_t _len) const // OutOfRangeException
			{
				return memm::template toint_limit<qword>(begin(), _len, _radix);
			}
			llong to_llong_limit_x(uint _radix, size_t _len) const // OutOfRangeException
			{
				return memm::template toint_limit<llong>(begin(), _len, _radix);
			}
			dword to_uint_limit_x(uint _radix, size_t _len) const // OutOfRangeException
			{
				return memm::template toint_limit<dword>(begin(), _len, _radix);
			}
			long to_int_limit_x(uint _radix, size_t _len) const // OutOfRangeException
			{
				return memm::template toint_limit<long>(begin(), _len, _radix);
			}
			uintp to_uintp_limit_x(uint _radix, size_t _len) const // OutOfRangeException
			{
				return memm::template toint_limit<uintp>(begin(), _len, _radix);
			}
			intp to_intp_limit_x(uint _radix, size_t _len) const // OutOfRangeException
			{
				return memm::template toint_limit<intp>(begin(), _len, _radix);
			}

			qword to_qword(uint _radix = 10) const noexcept
			{
				return to_qword_x(_radix, size());
			}
			llong to_llong(uint _radix = 10) const noexcept
			{
				return to_llong_x(_radix, size());
			}
			dword to_uint(uint _radix = 10) const noexcept
			{
				return to_uint_x(_radix, size());
			}
			long to_int(uint _radix = 10) const noexcept
			{
				return to_int_x(_radix, size());
			}
			uintp to_uintp(uint _radix = 10) const noexcept
			{
				return to_uintp_x(_radix, size());
			}
			intp to_intp(uint _radix = 10) const noexcept
			{
				return to_intp_x(_radix, size());
			}
			float to_ufloat() const noexcept
			{
				return to_ufloat_x(size());
			}
			float to_float() const noexcept
			{
				return to_float_x(size());
			}

			qword to_qword_limit(uint _radix = 10) const
			{
				return to_qword_limit_x(_radix, size());
			}
			llong to_llong_limit(uint _radix = 10) const
			{
				return to_llong_limit_x(_radix, size());
			}
			dword to_uint_limit(uint _radix = 10) const
			{
				return to_uint_limit_x(_radix, size());
			}
			long to_int_limit(uint _radix = 10) const
			{
				return to_int_limit_x(_radix, size());
			}
			dword to_uintp_limit(uint _radix = 10) const
			{
				return to_uintp_limit_x(_radix, size());
			}
			long to_intp_limit(uint _radix = 10) const
			{
				return to_intp_limit_x(_radix, size());
			}

			qword to_qword_l(size_t _len, uint _radix) const noexcept
			{
				return to_qword_x(_radix, math::min(_len, size()));
			}
			llong to_llong_l(size_t _len, uint _radix) const noexcept
			{
				return to_llong_x(_radix, math::min(_len, size()));
			}
			dword to_uint_l(size_t _len, uint _radix) const noexcept
			{
				return to_uint_x(_radix,math::min(_len, size()));
			}
			long to_int_l(size_t _len, uint _radix) const noexcept
			{
				return to_int_x(_radix,math::min(_len, size()));
			}
			dword to_uintp_l(size_t _len, uint _radix) const noexcept
			{
				return to_uintp_x(_radix, math::min(_len, size()));
			}
			long to_intp_l(size_t _len, uint _radix) const noexcept
			{
				return to_intp_x(_radix, math::min(_len, size()));
			}
			float to_ufloat_l(size_t _len) const noexcept
			{
				return to_ufloat_x(math::min(_len, size()));
			}
			float to_float_l(size_t _len) const noexcept
			{
				return to_float_x(math::min(_len, size()));
			}
			bool numberonly() const noexcept
			{
				return numberonly_x(size());
			}

			bool isdbcs(size_t n) const noexcept
			{
				return memm::isdbcs(begin(),n);
			}
			intp compare(Ref _str) const noexcept
			{
				size_t len1 = size();
				size_t len2 = _str.size();

				int order = memm::compare(begin(), _str.begin(), math::min(len1, len2));
				if (order == 0) return len1 - len2;
				return order;
			}
			bool equals_i(Ref _v) const noexcept
			{
				return equals_ix(_v, size());
			}
			bool equals_ix(Ref _v, size_t _len) const noexcept
			{
				size_t len2 = _v.size();
				if (_len != len2) return false;
				else return memm::equals_i(begin(), _v.begin(), _len);
			}

			Ref trim() const noexcept
			{
				Ref beg = find_ny(WHITE_SPACE);
				if (beg == nullptr)
					return Ref(end(), end());
				Ref end = find_nry(WHITE_SPACE);
				return beg.cut(end+1);
			}

			constexpr TextBuffer() noexcept = default;
			constexpr TextBuffer(const TextBuffer&) = default;
			constexpr TextBuffer(TextBuffer&&) = default;
			explicit TextBuffer(const Component* str) noexcept
				:Super(str, memm::pos(str, 0))
			{
			}

			explicit operator std::basic_string<InternalComponent>() noexcept
			{
				return std::basic_string<InternalComponent>(begin(), size());
			}
		};

		template <class Derived, class Info, class Next>
		class WTextBuffer:public TextBuffer<Derived, Info, Next>
		{
			CLASS_HEADER(WTextBuffer, TextBuffer<Derived, Info, Next>);
		public:
			INHERIT_COMPONENT();

			using Super::Super;
			using Super::begin;
			using Super::end;
			using Super::size;

			void toLowerCase() noexcept
			{
				_assert(!std::is_class<Component>::value);
				memm::tolower(begin(), size());
			}
			void toUpperCase() noexcept
			{
				_assert(!std::is_class<Component>::value);
				memm::toupper(begin(), size());
			}
		};

		template <class Derived, class Info, typename Next>
		const typename Info::Component (&TextBuffer<Derived, Info, Next>::WHITE_SPACE)[5] = _pri_::StaticText<typename Info::Component>::WHITE_SPACE;
	}
}