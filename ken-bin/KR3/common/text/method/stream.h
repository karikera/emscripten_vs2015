#pragma once

#include "../../io/istream.h"

namespace kr
{
	namespace ary
	{
		namespace method
		{
			template <class Parent>
			class IStreamMethod
				: public InStream<IStreamMethod<Parent>, typename Parent::Component, StreamInfo<true, Parent>>
			{
				CLASS_HEADER(IStreamMethod, InStream<IStreamMethod<Parent>, typename Parent::Component, StreamInfo<true, Parent>>);
			public:
				INHERIT_ARRAY();
				
				using Super::Super;
				using Super::addBegin;
				using Super::setBegin;

				using Parent::begin;
				using Parent::end;
				using Parent::size;
				using Parent::empty;

				Self& operator ++() noexcept
				{
					addBegin(1);
					return *static_cast<Self*>(this);
				}
				Ref operator ++(int) noexcept
				{
					Ref out = *static_cast<Self*>(this);
					addBegin(1);
					return out;
				}
				Self& operator --() noexcept
				{
					addBegin(-1);
					return *static_cast<Self*>(this);
				}
				Ref operator --(int) noexcept
				{
					Ref out = *static_cast<Self*>(this);
					addBegin(-1);
					return out;
				}
				Self& operator +=(intp value) noexcept
				{
					addBegin(value);
					return *static_cast<Self*>(this);
				}
				Self& operator -=(intp value) noexcept
				{
					addBegin(-value);
					return *static_cast<Self*>(this);
				}

				constexpr IStreamMethod() noexcept = default;
				constexpr IStreamMethod(const IStreamMethod&) = default;
				constexpr IStreamMethod(IStreamMethod&&) noexcept = default;
				template <class _Derived, class _Parent>
				constexpr IStreamMethod(const InStream<_Derived, Component, StreamInfo<true, _Parent>> & data) noexcept
					:Super(data.begin(), data.end())
				{
				}
			};
			template <class Parent>
			class OStreamMethod 
				: public OutStream<OStreamMethod<Parent>, typename Parent::Component, StreamInfo<true, Parent>>
			{
				CLASS_HEADER(OStreamMethod, OutStream<OStreamMethod<Parent>, typename Parent::Component, StreamInfo<true, Parent>>);
			public:
				INHERIT_ARRAY();

				using Super::Super;
				using Super::end;
				using Super::limit;

				constexpr OStreamMethod() noexcept = default;
				constexpr OStreamMethod(const OStreamMethod&) = default;
				constexpr OStreamMethod(OStreamMethod&&) = default;
				template <class _Derived, class _Parent>
				constexpr OStreamMethod(const OutStream<_Derived, Component, StreamInfo<true, _Parent>> & data) noexcept
					:Super(data.end(), data.limit())
				{
				}
			};
			template <class Parent>
			class IRStreamMethod
				: public Parent
			{
				CLASS_HEADER(IRStreamMethod, Parent);
			public:
				INHERIT_ARRAY();

				using Super::Super;
				using Super::begin;
				using Super::end;
				using Super::setEnd;
				using Super::empty;

				void skipBack() // EofException
				{
					if (empty())
						throw EofException();
					setEnd(end() - 1);
				}
				InternalComponent readBack() // EofException
				{
					if (empty())
						throw EofException();
					const InternalComponent * comp = (InternalComponent *)end() - 1;
					setEnd((Component*)comp);
					return *comp;
				}
				void cut_self(const ComponentRef* newend) noexcept
				{
					_assert(begin() <= newend && newend <= end());
					setEnd((Component*)newend);
				}
				void cut_self(Ref _v) noexcept
				{
					return cut_self(_v.begin());
				}
				void cut_self(size_t _len) noexcept
				{
					return cut_self(math::min(begin() + _len, end()));
				}

			};
		}
	}
}