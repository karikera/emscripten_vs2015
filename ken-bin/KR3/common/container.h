#pragma once

#include "../main.h"
#include "../meta/attacher.h"
#include "../meta/value.h"
#include "../meta/if.h"
#include "memlib.h"

namespace kr
{
	namespace _pri_
	{
		meta::bool_false isOStreamHelper(void*);
		template <typename Derived, typename Component, class Info> meta::bool_true isOStreamHelper(OutStream<Derived, Component, Info>*);

		meta::bool_false isIStreamHelper(void*);
		template <typename Derived, typename Component, class Info> meta::bool_true isIStreamHelper(InStream<Derived, Component, Info>*);

		meta::bool_false isContainerHelper(void*);
		template <typename C, bool rdonly, class Parent> meta::bool_true isContainerHelper(Container<C, rdonly, Parent>*);

		meta::bool_false isBufferHelper(void*);
		template <typename C, class Info> meta::bool_true isBufferHelper(Bufferable<C, Info>*);
	}

	template <typename T> struct IsOStream : decltype(_pri_::isOStreamHelper((T*)0)) {};
	template <typename T> struct IsIStream : decltype(_pri_::isIStreamHelper((T*)0)) {};
	template <typename T> struct IsContainer : decltype(_pri_::isContainerHelper((T*)0)) {};
	template <typename T> struct IsBuffer : decltype(_pri_::isBufferHelper((T*)0)) {};


	template <typename T> struct internal_component
	{
		using type = T;
	};
	template <> struct internal_component<void>
	{
		using type = byte;
	};
	template <typename T>
	using internal_component_t = typename internal_component<T>::type;


	template <typename C, bool rdonly, class Parent> 
	class Container: public Parent
	{
	public:
		static constexpr bool readonly = rdonly;
		static constexpr bool is_string = 
			is_same<C, char>::value ||
			is_same<C, char16>::value ||
			is_same<C, char32>::value ||
			is_same<C, wchar>::value;
		using Component = C;
		using ComponentRef = meta::if_type<rdonly, const C, C>;
		using InternalComponent = internal_component_t<C>;
		using InternalComponentRef = meta::if_type<rdonly, const InternalComponent, InternalComponent>;
		using memm = memt<sizeof(internal_component_t<C>)>;

		using Alc = Array<C>;
		using Ref = RefArray<C>;
		using WRef = WRefArray<C>;
		using Wri = ArrayWriter<C>;

		using Parent::Parent;
	};
	template <bool rdonly, class Parent> 
	class Container<AutoComponent, rdonly, Parent> : public Parent
	{
	public:
		static constexpr bool readonly = rdonly;
		static constexpr bool is_string = true;
		using Component = AutoComponent;
		using InternalComponent = AutoComponent;
		using ComponentRef = meta::if_type<rdonly, const AutoComponent, AutoComponent>;
		using InternalComponentRef = ComponentRef;
		using memm = void;

		using Alc = void;
		using Ref = void;
		using WRef = void;
		using Wri = void;

		using Parent::Parent;
	};

	template <bool _accessable, class Parent>
	class StreamInfo :public Parent
	{
	public:
		static constexpr bool accessable = _accessable;
		using BaseStream = Undefined;

		using Parent::Parent;
	};

}

#define INHERIT_COMPONENT() \
	static_assert(kr::IsContainer<Super>::value, "Super is not Container"); \
	using typename Super::Component;\
	using typename Super::ComponentRef;\
	using typename Super::InternalComponent;\
	using typename Super::InternalComponentRef;\
	using Alc = typename Super::Alc;\
	using Ref = typename Super::Ref;\
	using Wri = typename Super::Wri;\
	using WRef = typename Super::WRef;\
	using typename Super::memm;


