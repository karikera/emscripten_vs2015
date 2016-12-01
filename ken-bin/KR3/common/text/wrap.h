#pragma once

#include "container.h"
#include "method/buffer_i.h"
#include "method/buffer_io.h"
#include "method/stream.h"
#include "../../meta/ordering.h"

namespace kr
{
	namespace ary
	{
		namespace _pri_
		{
			template <class Data, typename C> 
			class WrapImpl :
				public meta::crtp_order<Data>::
				template attach<method::OStreamMethod, Data::extendable>::
				template attach<method::IStreamMethod, Data::movable>::
				template attach<method::IRStreamMethod, Data::movable>::
				template attach<method::BufferIMethod, Data::readable>::
				template attach<method::BufferIOMethod, Data::extendable && Data::readable>::
				type
			{
				CLASS_HEADER(WrapImpl,
					typename meta::crtp_order<Data>::
					template attach<method::OStreamMethod, Data::extendable>::
					template attach<method::IStreamMethod, Data::movable>::
					template attach<method::IRStreamMethod, Data::movable>::
					template attach<method::BufferIMethod, Data::readable>::
					template attach<method::BufferIOMethod, Data::extendable && Data::readable>::
					type);
			public:
				INHERIT_ARRAY();
				static_assert(is_same<C, Component>::value, "Component type not matching");

				using Data::isNull;
				using Super::equals;

				using Super::Super;
				using Super::print;

				constexpr WrapImpl() noexcept = default;
				constexpr WrapImpl(const WrapImpl&) = default;
				constexpr WrapImpl(WrapImpl&&) = default;

				bool operator !=(nullptr_t) const noexcept
				{
					return !isNull();
				}
				bool operator ==(nullptr_t) const noexcept
				{
					return isNull();
				}
				friend bool operator !=(nullptr_t, const WrapImpl& _this) noexcept
				{
					return !_this.isNull();
				}
				friend bool operator ==(nullptr_t, const WrapImpl& _this) noexcept
				{
					return _this.isNull();
				}
				bool operator !=(Ref ref) const noexcept
				{
					return !equals(ref);
				}
				bool operator ==(Ref ref) const noexcept
				{
					return equals(ref);
				}
				template <typename T> WrapImpl& operator << (const T& arrayable) // NotEnoughSpaceException
				{
					print(arrayable);
					return *this;
				}

				static inline void* operator new(size_t, WrapImpl* _Where) noexcept
				{
					return _Where;
				}
				static inline void operator delete(void*, WrapImpl*) noexcept
				{
				}
				WrapImpl & operator =(const WrapImpl & _copy)
				{
					this->~WrapImpl();
					new (this) WrapImpl(_copy);
					return *this;
				}
				WrapImpl & operator =(WrapImpl && _move) noexcept
				{
					this->~WrapImpl();
					new (this) WrapImpl(move(_move));
					return *this;
				}

				template <size_t _len>
				constexpr WrapImpl(const InternalComponent(&_str)[_len]) noexcept
					: Super(_str, _len - Data::is_string)
				{
				}
			};
		}
	}
}
