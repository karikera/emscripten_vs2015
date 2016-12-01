#pragma once

#include "../container.h"
#include "../../meta/types.h"

namespace kr
{
	namespace _pri_
	{
		template <class Derived, typename C, class Parent>
		class OStream_cmpAccessable<Derived, C, StreamInfo<false, Parent>>
			:public Container<C, false, StreamInfo<false, Parent>>
		{
			CLASS_HEADER(OStream_cmpAccessable, Container<C, false, StreamInfo<false, Parent>>);
		public:
			using Super::Super;
			using typename Super::Component;
			using typename Super::InternalComponent;

			inline void write(const C * comp, size_t size) // NotEnoughSpaceException
			{
				return static_cast<Derived*>(this)->writeImpl(comp, size);
			}
			inline void writeFill(const InternalComponent & data, size_t size) // NotEnoughSpaceException
			{
				TmpArray<Component> arr(size);
				arr.fill(data);
				write(arr.data(), size);
			}
		};;

		template <class Derived, typename C, class Parent>
		class OStream_cmpAccessable<Derived, C, StreamInfo<true, Parent>>
			:public Container<C, false, StreamInfo<true, Parent>>
		{
			CLASS_HEADER(OStream_cmpAccessable, Container<C, false, StreamInfo<true, Parent>>);
		public:
			INHERIT_COMPONENT();

		private:
			inline Derived* derived() noexcept
			{
				return static_cast<Derived*>(this);
			}
			inline ComponentRef* ownerEnd() noexcept
			{
				return static_cast<Derived*>(this)->end();
			}

		public:
			using Super::Super;
			using Super::end;

			// capacity를 늘리지 않고, 예외 없이 강제로 준비한다.
			inline Component* prepareForce(size_t size) noexcept
			{
				Component * e = end();
				derived()->_addEnd(size);
				mema::ctor(e, size);
				return e;
			}
			inline Component* prepare(size_t size) // NotEnoughSpaceException
			{
				Component * e = derived()->_extend(size);
				mema::ctor(e, size);
				return e;
			}
			inline void writeForce(const InternalComponent & data) // NotEnoughSpaceException
			{
				*derived()->end() = data;
				derived()->_addEnd(1);
			}
			inline void writeForce(const Component * data, size_t size) // NotEnoughSpaceException
			{
				Component * e = derived()->end();
				derived()->_addEnd(size);
				mema::ctor_copy((InternalComponent*)e, (const InternalComponent*)data, size);
			}
			inline void write(const Component * data, size_t size) // NotEnoughSpaceException
			{
				Component * e = derived()->_extend(size);
				mema::ctor_copy((InternalComponent*)e, (const InternalComponent*)data, size);
			}
			inline void writeFill(const InternalComponent & data, size_t size) // NotEnoughSpaceException
			{
				Component * e = derived()->_extend(size);
				mema::ctor_fill(e, data, size);
			}
			template <typename T>
			inline void writeas(const T &value)
			{
				static_assert(sizeof(T) % sizeof(Component) == 0, "Size of T must aligned by size of component");
				return write((Component*)&value, sizeof(T) / sizeof(Component));
			}
			// without constructor
			inline Component * padding(size_t size) // NotEnoughSpaceException
			{
				if (size == 0) return ownerEnd();
				return derived()->_padding(size);
			}
			// without constructor
			inline void commit(size_t sz)
			{
				if (sz == 0)
					return;
				derived()->_addEnd(sz);
			}
			template <typename ... ARGS>
			void prints(const ARGS & ... args) // NotEnoughSphaceException
			{
				meta::types<bufferize_t<ARGS, Component> ...> datas = { ((bufferize_t<ARGS, Component>)args) ... };
				size_t size = 0;
				datas.value_loop([&size](auto & data) { size += data.size(); });
				padding(size);
				datas.value_loop([this](auto & data) { data.writeTo(derived()); });
			}
		};

		template <class Derived, typename C, typename Info>
		class OStream_cmpComponent :public OStream_cmpAccessable<Derived, C, Info>
		{
			CLASS_HEADER(OStream_cmpComponent, OStream_cmpAccessable<Derived, C, Info>);
		public:
			INHERIT_COMPONENT();
			using Super::Super;
		};

		template <class Derived, class Parent>
		class OStream_cmpComponent<Derived, AutoComponent, StreamInfo<false, Parent>>;
	}

	template <class Derived, typename Component, typename Info>
	class OutStream :public _pri_::OStream_cmpComponent<Derived, Component, Info>
	{
		CLASS_HEADER(OutStream, _pri_::OStream_cmpComponent<Derived, Component, Info>);

	public:
		INHERIT_COMPONENT();

		using Super::Super;
		using Super::write;

		void push(const InternalComponent& data) // NotEnoughSpaceException
		{
			write((const Component*)&data, 1);
		}
		void write(const InternalComponent & data) // NotEnoughSpaceException
		{
			write((const Component*)&data, 1);
		}
		void write(Ref data) // NotEnoughSpaceException
		{
			write(data.begin(), data.size());
		}

		template <typename T>
		void print(const T & v)// NotEnoughSpaceException
		{
			using buffer_ref_t = bufferize_t<T, Component>;
			using buffer_t = remove_constref_t<buffer_ref_t>;
			using buffer_component_t = typename buffer_t::Component;
			static_assert_with_type(
				(is_same<buffer_component_t, AutoComponent>::value || is_same<buffer_component_t, Component>::value),
				buffer_ref_t, buffer_t, buffer_component_t>::value); // Unmatch component type
			buffer_ref_t(v).writeTo(this);
		}

		template <typename T>
		OutStream& operator << (const T& arrayable) // NotEnoughSpaceException
		{
			print(arrayable);
			return *this;
		}
		OutStream& operator << (OutStream& (*fn)(OutStream&))
		{
			_assert(fn != nullptr);
			return ((*fn)(*this));
		}
	};

	namespace io
	{
		template <typename C>
		class SizeOStream :public OutStream<SizeOStream<C>, C>
		{
		private:
			size_t m_size;

		public:
			SizeOStream() noexcept
			{
				m_size = 0;
			}

			size_t size() const noexcept
			{
				return m_size;
			}
			SizeOStream & operator += (size_t _sz) noexcept
			{
				m_size += _sz;
				return *this;
			}
			void writeImpl(const C *, size_t _sz) noexcept
			{
				m_size += _sz;
			}
		};
	}
}
