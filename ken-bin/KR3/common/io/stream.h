#pragma once

#include "../container.h"
#include "istream.h"
#include "ostream.h"


namespace kr
{
	namespace io
	{
		template <class Derived, typename C = void, typename _Info = StreamInfo<>>
		class Streamable :public OutStream<Derived, C, StreamInfo<_Info::accessable, InStream<Derived, C, _Info>>>
		{
			CLASS_HEADER(Streamable, OutStream<Derived, C, StreamInfo<_Info::accessable, InStream<Derived, C, _Info>>>);
		public:
			INHERIT_COMPONENT();
			using Super::Super;
		};
		template <class Derived, typename _Info>
		class Streamable<Derived, void, _Info>:public OutStream<Derived, void, StreamInfo<_Info::accessable, InStream<Derived, void, _Info>>>
		{
			CLASS_HEADER(Streamable, OutStream<Derived, void, StreamInfo<_Info::accessable, InStream<Derived, void, _Info>>>);
		public:
			INHERIT_COMPONENT();
			using Super::Super;
			template <typename NC> io::RetypeStream<Derived, NC> * retype()
			{
				return static_cast<io::RetypeStream<Derived, NC>*>(this);
			}
		};
		template <class Base, typename C>
		class RetypeStream: public Streamable<RetypeStream<Base, C>, C, StreamInfo<Base::accessable, Base>>
		{
			CLASS_HEADER(RetypeStream, Streamable<RetypeStream<Base, C>, C, StreamInfo<Base::accessable, Base>>);
		public:
			INHERIT_COMPONENT();

		private:
			using BaseInternalComponent = typename Base::InternalComponent;
			using BaseComponent = typename Base::Component;
			static constexpr size_t _thisSize = sizeof(InternalComponent);
			static constexpr size_t _baseSize = sizeof(BaseInternalComponent);
			static_assert(_thisSize == _baseSize, "size unmatch");

		public:
			using Super::Super;
			using BaseStream = Base;

			void writeImpl(const C * data, size_t size)
			{
				return Base::writeImpl((BaseComponent*)data, size);
			}
			size_t readImpl(C * data, size_t size)
			{
				return Base::readImpl((BaseComponent*)data, size);
			}
		};

	}
}
