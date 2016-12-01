#include "class.h"
#pragma once

template <typename T>
inline kr::JsClass<T>::JsClass(Text16 _name, int internalFieldCount)
{
	setClass<T>(_name, internalFieldCount);
	T::initMethods(this);
}
template <typename T>
inline kr::JsClass<T>::JsClass(Text16 _name, V8Class * parent)
{
	setClass<T>(_name, parent);
	T::initMethods(this);
}

template <typename T>
inline kr::V8Class kr::JsClass<T>::createChild()
{
	return V8Class::createChild<T>();
}

template <typename T>
template <typename P>
inline void kr::JsClass<T>::setAccessor(Text16 _name, P(T::*v))
{
	auto get = [v](V8Object _this)->JsAny {
		T * _nthis = _this.getNativeObject<T>();
		if (_nthis == nullptr)
			return undefined;
		return (const P&)(_nthis->*v);
	};
	auto set = [v](V8Object _this, JsAny _nv) {
		T * _nthis = _this.getNativeObject<T>();
		if (_nthis == nullptr)
			return;
		_nthis->*v = _nv.cast<P>();
	};
	V8Class::setAccessor(_name, get, set);
}

template <typename T>
template <typename P>
inline void kr::JsClass<T>::setReadOnlyAccessor(Text16 _name, P(T::*v))
{
	auto get = [v](V8Object _this)->JsAny {
		T * _nthis = _this.getNativeObject<T>();
		if (_nthis == nullptr) return undefined;
		return (const P&)(_nthis->*v);
	};
	V8Class::setAccessor(_name, get);
}

template <typename T>
template <typename P>
inline void kr::JsClass<T>::setMethodAccessor(Text16 _name, P(T::*get)())
{
	V8Class::setReadOnlyAccessor(_name, [get](V8Object _this)->JsAny{
		T * _nthis = _this.getNativeObject<T>();
		if (_nthis == nullptr) return undefined;
		return (_nthis->*get)();
	});
}

template <typename T>
template <typename P>
inline void kr::JsClass<T>::setMethodAccessor(Text16 _name, P(T::*get)(), void(T::*set)(P))
{
	V8Class::setAccessor(_name, [get](V8Object _this)->JsAny {
		T * _nthis = _this.getNativeObject<T>();
		if (_nthis == nullptr) return undefined;
		return (_nthis->*get)();
	}, [set](V8Object _this, JsAny nv) {
		T * _nthis = _this.getNativeObject<T>();
		if (_nthis == nullptr) return;
		return (_nthis->*set)(nv.cast<P>());
	});
}

template <typename T>
template <typename P>
inline void kr::JsClass<T>::setNullableMethodAccessor(Text16 _name, P(T::*get)())
{
	V8Class::setReadOnlyAccessor(_name, [get](V8Object _this)->JsAny {
		T * _nthis = _this.getNativeObject<T>();
		return (_nthis->*get)();
	});
}

template <typename T>
template <typename P>
inline void kr::JsClass<T>::setNullableMethodAccessor(Text16 _name, P(T::*get)(), void(T::*set)(P))
{
	V8Class::setAccessor(_name, [get](V8Object _this)->JsAny {
		T * _nthis = _this.getNativeObject<T>();
		return (_nthis->*get)();
	}, [set](V8Object _this, JsAny nv) {
		T * _nthis = _this.getNativeObject<T>();
		return (_nthis->*set)(nv.cast<P>());
	});
}

template <typename T>
inline void kr::JsClass<T>::setMethodRaw(Text16 _name, JsAny(T::* func)(const JsArguments &))
{
	set(_name, JsFunction([func](const JsArguments & args)->JsAny {
		T * _nthis = args.getThis().getNativeObject<T>();
		if (_nthis == nullptr) return undefined;
		return (_nthis->*func)(args);
	}));
}

template <typename T>
template <typename P>
inline void kr::JsClass<T>::setIndexAccessor(P(T::* get)(uint32_t))
{
	V8Class::setReadOnlyIndexAccessor(JsIndexAccessor([get](V8Object _this, uint32_t idx)->JsAny {
		T * _nthis = _this.getNativeObject<T>();
		if (_nthis == nullptr) return undefined;
		return (_nthis->*get)(idx);
	}));
}

template <typename T>
template <typename P>
inline void kr::JsClass<T>::setIndexAccessor(P(T::* get)(uint32_t), void(T::* set)(uint32_t, P))
{
	V8Class::setIndexAccessor(JsIndexAccessor([get](V8Object _this, uint32_t idx)->JsAny {
		T * _nthis = _this.getNativeObject<T>();
		if (_nthis == nullptr) return undefined;
		return (_nthis->*get)(idx);
	}, [set](V8Object _this, unsigned long idx, JsAny value) {
		T * _nthis = args.getThis().getNativeObject<T>();
		if (_nthis == nullptr) return;
		return (_nthis->*set)(idx, value);
	}));
}

template <typename T>
template <typename RET, typename ... ARGS>
inline void kr::JsClass<T>::setMethod(Text16 _name, RET(T::* func)(ARGS ...))
{
	set(_name, JsFunction([func](const JsArguments & args)->JsAny {
		return JsMeta<decltype(func)>::call(func, args);
	}));
}

// 객체를 생성합니다
// 기본적으로 Weak 상태로 생성되어, GC에 의하여 지워질 수 있습니다.
template <typename T>
template <typename ... ARGS>
T* kr::JsClass<T>::newInstance(const ARGS & ... args) const
{
	meta::array<JsAny, sizeof...(args)> list = { args ... };
	return static_cast<T*>(newInstanceRawPtr(RefArray<JsAny>(list, endof(list))));
}

template <typename T>
inline void kr::V8Handle<v8::FunctionTemplate>::setClass(Text16 _name, int internalFieldCount)
{
	_init(_name, internalFieldCount, [](const JsArguments & arguments)->NativeObject* {
		return _newAligned(T, arguments);
	});
}
template <typename T>
inline void kr::V8Handle<v8::FunctionTemplate>::setClass(Text16 _name, V8Class * parent)
{
	_init(_name, parent, [](const JsArguments & arguments)->NativeObject* { 
		return _newAligned(T, arguments);
	});
}
template <typename T>
inline kr::V8Class kr::V8Handle<v8::FunctionTemplate>::createChild()
{
	return _createChild([](const JsArguments & arguments)->NativeObject* { return _new T(arguments); });
}
template <typename LAMBDA>
inline void kr::V8Handle<v8::FunctionTemplate>::setStaticMethodRaw(Text16 _name, LAMBDA lambda)
{
	setStatic(_name, JsFunction(lambda));
}
template <typename T, typename P>
inline void kr::V8Handle<v8::FunctionTemplate>::setMethodAccessor(Text16 _name, P(T::*get)())
{
	setReadOnlyAccessor(_name, [get](V8Object _this)->JsAny {
		T * _nthis = _this.getNativeObject<T>();
		if (_nthis == nullptr) return undefined;
		return (_nthis->*get)();
	});
}
template <typename T, typename P>
inline void kr::V8Handle<v8::FunctionTemplate>::setMethodAccessor(Text16 _name, P(T::*get)(), void(T::*set)(P))
{
	setAccessor(_name, [get](V8Object _this)->JsAny {
		T * _nthis = _this.getNativeObject<T>();
		if (_nthis == nullptr) return undefined;
		return (_nthis->*get)();
	}, [set](V8Object _this, JsAny nv) {
		T * _nthis = _this.getNativeObject<T>();
		if (_nthis == nullptr) return;
		return (_nthis->*set)(nv.cast<P>());
	});
}

template <typename T, typename RET, typename ... ARGS>
inline void kr::V8Handle<v8::FunctionTemplate>::setMethod(Text16 _name, RET(T::* func)(ARGS ...))
{
	set(_name, JsFunction([func](const JsArguments & args)->JsAny {
		return JsMeta<decltype(func)>::call(func, args);
	}));
}

template <typename LAMBDA>
inline void kr::V8Handle<v8::FunctionTemplate>::setStaticMethod(Text16 _name, LAMBDA lambda)
{
	setStatic(_name, JsFunction::make(lambda));
}
