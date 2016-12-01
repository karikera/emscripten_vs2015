#pragma once

#include <type_traits>
#include "jsvar.h"
#include "nativeclass.h"

namespace kr
{
	namespace _pri_
	{
		struct JsCast
		{
			template <typename To, typename From>
			struct ToOuter
			{
				static inline To toOuter(const From & _value) noexcept
				{
					return (To)_value;
				}
				static inline To toOuter(From && _value) noexcept
				{
					return To(move(_value));
				}
			};
			template <typename To, typename From>
			struct ToOuter<To*, From>
			{
				static inline To* toOuter(const V8Value & _value) noexcept
				{
					V8Object obj = (V8Object)_value;
					if (obj.isEmpty())
						return nullptr;
					return static_cast<To*>(obj.getNativeObject());
				}
			};
			template <typename From>
			struct ToOuter<void*, From>
			{
				static inline void* toOuter(void * _value) noexcept
				{
					return _value;
				}
			};
			template <typename T,typename TI>
			static inline T toOuter(const TI & _value) noexcept
			{
				return ToOuter<T, TI>::toOuter(_value);
			}
			template <typename T, typename TI>
			static inline T toOuter(TI && _value) noexcept
			{
				return ToOuter<T, TI>::toOuter(move(_value));
			}
#define CASTER(from, to) \
inline static to toInner(from _value) noexcept { return (to)_value; }
#define CASTER_COPY(from, to) \
inline static to toInner(const from & _value) noexcept { return (to)_value; }
#define CASTER_MOVE(from, to) \
inline static to toInner(const from & _value) noexcept { return (to)_value; }\
inline static to toInner(from && _value) noexcept { return to(move(_value)); }\

			CASTER(char, int);
			CASTER(short, int);
			CASTER(int, int);
			CASTER(long, int);
			CASTER(long long, double);
			CASTER(unsigned char, int);
			CASTER(unsigned short, int);
			CASTER(unsigned int, int);
			CASTER(unsigned long, double);
			CASTER(unsigned long long, double);
			CASTER(bool, bool);
			CASTER(void*, void*);
			CASTER(double, double);
			CASTER(float, double);
			CASTER(nullptr_t, nullptr_t);
			CASTER(undefined_t, undefined_t);
			CASTER(Text16, JsString);
			CASTER_MOVE(JsString, JsString);
			CASTER_MOVE(JsFunction, JsFunction);
			CASTER_MOVE(JsAny, JsAny);
#undef CASTER
#undef CASTER_MOVE
#undef CASTER_COPY
			inline static JsString toInner(Text _value) noexcept { return (JsString)_value; }
			inline static JsString toInner(const AText &_value) noexcept { return (Text)_value; }
			inline static JsString toInner(const AText16 &_value) noexcept { return (Text16)_value; }
			template <typename T>
			inline static V8Value toInner(const V8Handle<T> & _value) noexcept { return _value; }
			template <typename T>
			inline static V8Value toInner(V8Handle<T> && _value) noexcept { return move(_value); }
			template <typename T>
			inline static V8Value toInner(V8Handle<T> * _value) noexcept {
				if (_value == nullptr)
					return V8Value();
				return *_value;
			}
			template <typename FUNC>
			inline static JsFunction toInner(const JsNativeFunction<FUNC> & _value) noexcept { return _value; }
			template <typename FUNC>
			inline static JsFunction toInner(JsNativeFunction<FUNC> && _value) noexcept { return move(_value); }

			template <typename T>
			inline static T defaultValue() noexcept
			{
				using type = decltype(toInner(*(T*)0));
				return toOuter<T>(defaultValue<type>());
			}
			template <>
			inline static void defaultValue<void>() noexcept
			{
			};
#define DEFAULT(type, v)	template <> inline static type defaultValue<type>() noexcept { return v; }
			DEFAULT(int, 0);
			DEFAULT(double, NAN);
			DEFAULT(bool, false);
			DEFAULT(void*, nullptr);
			DEFAULT(nullptr_t, nullptr);
			DEFAULT(undefined_t, undefined);
			DEFAULT(V8Value, V8Value());
			DEFAULT(JsString, JsString());
			DEFAULT(JsFunction, JsFunction());
			DEFAULT(JsAny, JsAny());
#undef DEFAULT
		};

		enum CastType
		{
			CastTypeSame,
			CastTypeCast,
			CastTypeDefault
		};
		template <CastType type, typename S, typename D> struct ComputeCastImpl;
		template <typename S, typename D> struct ComputeCastImpl<CastTypeCast, S, D>
		{
			static inline D cast(const S &v)
			{
				return (D)v;
			}
		};
		template <typename S, typename D> struct ComputeCastImpl<CastTypeSame, S, D>
		{
			static inline const D & cast(const S &v)
			{
				return v;
			}
		};
		template <typename S, typename D> struct ComputeCastImpl<CastTypeDefault, S, D>
		{
			static inline D cast(const S &)
			{
				return JsCast::defaultValue<D>();
			}
		};

		template <typename S, typename D> struct ComputeCast: public ComputeCastImpl<
			std::is_same<S, D>::value ? CastTypeSame :
			std::is_same<D, JsString>::value ? CastTypeCast :
			std::is_same<D, JsAny>::value ? CastTypeCast :
			(
				std::is_same<JsFunction, D>::value ||
				std::is_same<undefined_t, D>::value ||
				std::is_same<nullptr_t, D>::value ||
				std::is_same<V8Value, D>::value ||
				std::is_same<void*, D>::value ||
				std::is_same<S, JsFunction>::value ||
				std::is_same<S, undefined_t>::value ||
				std::is_same<S, nullptr_t>::value ||
				std::is_same<S, V8Value>::value ||
				std::is_same<S, void*>::value
			) ? CastTypeDefault : CastTypeCast, S, D>
		{
		};
		template <typename S> struct ComputeCast<S, bool>
		{
			static inline bool cast(const S &v) { return v != 0; }
		};
		template <> struct ComputeCast<JsFunction, bool>
		{
			static inline bool cast(const JsFunction &v) { return !v.isEmpty(); }
		};
		template <> struct ComputeCast<nullptr_t, bool>
		{
			static inline bool cast(nullptr_t) { return false; }
		};
		template <> struct ComputeCast<undefined_t, bool>
		{
			static inline bool cast(undefined_t) { return false; }
		};
		template <> struct ComputeCast<V8Value, bool>
		{
			static inline bool cast(const V8Value &v) { return !v.isEmpty(); }
		};
		template <> struct ComputeCast<JsString, bool>
		{
			static inline bool cast(const JsString &v) { return v.c_str() != nullptr; }
		};
		template <> struct ComputeCast<JsString, int>
		{
			static inline int cast(const JsString &v) { return v.to_int(); }
		};
		template <> struct ComputeCast<JsString, double>
		{
			static inline double cast(const JsString &v) { return v.to_float(); }
		};
	}
}

inline kr::JsAny::JsAny()
{
	m_type = JsTypeUndefined;
}
template <typename T> T* kr::JsAny::_as()
{
	return (T*)&m_data;
}
template <typename T> const T* kr::JsAny::_as() const
{
	return (const T*)&m_data;
}

template <typename T>
inline void kr::JsAny::_ctor(const T & value)
{
	using inner = decltype(_pri_::JsCast::toInner(value));
	m_type = GetJsTypeFromType<inner>::value;
	new(_as<inner>()) inner(_pri_::JsCast::toInner(value));
}
template <typename T>
inline void kr::JsAny::_ctor(T && value)
{
	using inner = decltype(_pri_::JsCast::toInner((T&&)value));
	m_type = GetJsTypeFromType<inner>::value;
	new(_as<inner>()) inner(_pri_::JsCast::toInner((T&&)value));
}
template <>
inline void kr::JsAny::_ctor<kr::JsAny>(const JsAny & value)
{
	m_type = value.m_type;
	new(this) JsAny(value);
}
template <>
inline void kr::JsAny::_ctor<kr::JsAny>(JsAny && value)
{
	m_type = value.m_type;
	new(this) JsAny(move(value));
}
template <typename T> inline kr::JsAny::JsAny(const T & value)
{
	_ctor(value);
}
template <typename T> inline kr::JsAny::JsAny(T & value)
{
	_ctor((const T &)value);
}
template <typename T> inline kr::JsAny::JsAny(T && value)
{
	_ctor(move(value));
}
inline kr::JsAny::JsAny(const JsAny& v)
{
	m_type = v.m_type;
	EZV8_TYPE_CONSTLIZE(_ctor(*v._as<type>()));
}
inline kr::JsAny::JsAny(JsAny&& v)
{
	m_type = v.m_type;
	EZV8_TYPE_CONSTLIZE(_ctor(move(*v._as<type>())));
	v.m_type = JsTypeUndefined;
}
inline kr::JsAny::~JsAny()
{
	EZV8_TYPE_CONSTLIZE(_as<type>()->~type());
}

// 변수의 타입을 가져온다.
inline kr::JsType kr::JsAny::getType() const
{
	return m_type;
}

// 값을 가져온다. bool, int, float, double, AText, AText16, void* 만 지원된다.
// 타입이 다를경우 변환을 하여 가져오게 된다.
template <typename T> T kr::JsAny::cast() const
{
	typedef decltype(_pri_::JsCast::toInner(*(T*)0)) inner;
	EZV8_TYPE_CONSTLIZE(
		return _pri_::JsCast::toOuter<T>(_pri_::ComputeCast<type, inner>::cast(*_as<type>()));
	);
	return _pri_::JsCast::defaultValue<T>();
}

template <>
inline void kr::JsAny::cast<void>() const
{
}

// 내부에서 타입 확인 없이, 강제로 캐스트 하여 가져온다.
template <typename T> const T& kr::JsAny::as() const
{
	return *_as<T>();
}

// 내부에서 타입 확인 없이, 강제로 캐스트 하여 가져온다.
template <typename T> T& kr::JsAny::as()
{
	return *_as<T>();
}

inline kr::JsAny& kr::JsAny::operator =(const JsAny& _copy)
{
	this->~JsAny();
	new(this) JsAny(_copy);
	return *this;
}
inline kr::JsAny& kr::JsAny::operator =(JsAny&& _move)
{
	this->~JsAny();
	new(this) JsAny(move(_move));
	return *this;
}

inline bool kr::JsAny::operator ==(const JsAny & args) const
{
	if (m_type != args.getType()) return false;
	EZV8_TYPE_CONSTLIZE(return as<type>() == args.as<type>(); );
	return false;
}
inline bool kr::JsAny::operator !=(const JsAny & args) const
{
	return !(*this == args);
}

inline kr::JsString::JsString()
{
	m_data = nullptr;
}
inline kr::JsString::JsString(nullptr_t)
{
	m_data = nullptr;
}
inline kr::JsString::JsString(const JsString & _copy)
	:JsString((Text16)_copy)
{
}
inline kr::JsString::JsString(JsString && _move)
{
	m_data = _move.m_data;
	_move.m_data = nullptr;
}

inline kr::JsString::JsString(Text str, Charset cs) noexcept
{
	if (str == nullptr)
	{
		m_data = nullptr;
		return;
	}
	CHARSET_CONSTLIZE(cs, {
		MultiByteToUtf16<cs> utf16 = str;
		rawAlloc(utf16.size());
		utf16.copyTo(m_data);
	});
}
inline kr::JsString::JsString(Text16 str)
{
	if (str == nullptr)
	{
		m_data = nullptr;
		return;
	}
	size_t outsize = str.size();
	rawAlloc(outsize);
	memcpy(m_data, str.begin(), sizeof(char16)*outsize);
}
inline kr::JsString::JsString(int n)
	:JsString((Text16)(BText16<32>() << n))
{
}
inline kr::JsString::JsString(double n)
	: JsString((Text16)(TSZ16() << n))
{
}
inline kr::JsString::JsString(bool b)
	:JsString(b ? (Text16)u"true" : (Text16)u"false")
{
}
inline kr::JsString::JsString(undefined_t)
	:JsString((Text16)u"undefined")
{
}
inline kr::JsString::JsString(void *)
	: JsString((Text16)u"[[external]]")
{
}
inline kr::JsString::JsString(const JsFunction & obj)
	: JsString((Text16)u"[[native_function]]")
{
}
inline kr::JsString::~JsString()
{
	if(m_data == nullptr)
		return;
	rawFree();
}
inline char16 * kr::JsString::begin() noexcept
{
	return m_data;
}
inline char16 * kr::JsString::end() noexcept
{
	return m_data + length();
}
inline const char16 * kr::JsString::begin() const noexcept
{
	return m_data;
}
inline const char16 * kr::JsString::end() const noexcept
{
	return m_data + length();
}
inline void kr::JsString::alloc(size_t sz) noexcept
{
	if (m_data != nullptr)
		rawFree();
	rawAlloc(sz);
}
inline void kr::JsString::free() noexcept
{
	if (m_data != nullptr)
	{
		rawFree();
		m_data = nullptr;
	}
}
inline size_t kr::JsString::size() const noexcept
{
	return length();
}
inline kr::JsString& kr::JsString::operator =(const JsString& _copy)
{
	this->~JsString();
	new(this) JsString(_copy);
	return *this;
}
inline kr::JsString& kr::JsString::operator =(JsString&& _move)
{
	this->~JsString();
	new(this) JsString(move(_move));
	return *this;
}
inline kr::pcstr16 kr::JsString::c_str() const
{
	return m_data;
}
inline size_t kr::JsString::length() const
{
	if (m_data == nullptr)
		return 0;
	return *((size_t*)m_data - 1);
}
inline kr::JsString::operator kr::Text16() const noexcept
{
	if(m_data == nullptr)
		return nullptr;
	return Text16(c_str(), length());
}
inline kr::JsString::operator kr::AText() const noexcept
{
	return (Utf16ToAcp)(Text16)*this;
}
inline kr::JsString::operator kr::AText16() const noexcept
{
	return AText16(c_str(), length());
}


inline bool kr::JsString::operator ==(nullptr_t) const
{
	return c_str() == nullptr;
}
inline bool kr::JsString::operator !=(nullptr_t) const
{
	return c_str() != nullptr;
}
inline bool kr::JsString::operator ==(const JsString & args) const
{
	if (args.c_str() == nullptr)
	{
		if (c_str() == nullptr)
			return true;
		else
			return false;
	}
	else
	{
		if (c_str() == nullptr)
			return false;
		else
			return memcmp(c_str(), args.c_str(), length() * sizeof(char16)) == 0;
	}
}
inline bool kr::JsString::operator !=(const JsString & args) const
{
	return !(*this == args);
}

inline void kr::JsString::rawFree() noexcept
{
	ezv8_free((size_t*)m_data - 1);
}

inline void kr::JsString::rawAlloc(size_t sz) noexcept
{
	void * str = ezv8_malloc(sz * sizeof(char16) + sizeof(char16) + sizeof(size_t));
	*(size_t*)str = sz;
	m_data = (char16*)((size_t*)str + 1);
	m_data[sz] = L'\0';
}


inline kr::JsAny kr::V8Object::callRaw(Text16 name, JsArgumentsIn args) const
{
	return get(name).cast<V8Function>().callRaw(*this, args);
}
template <typename Class> inline void kr::V8Object::setClass()
{
	setClass(Class::getClassName(), &Class::classObject);
}
template <typename LAMBDA> inline void kr::V8Object::setFunctionRaw(Text16 name, LAMBDA lambda)
{
	set(name, JsFunction(lambda));
}

template <typename RET, typename ... ARGS> inline RET kr::V8Object::call(Text16 name, const ARGS & ... args) const
{
	JsAny args[] = { JsAny(args) ... };
	return callRaw(name, JsArgumentsIn(args)).cast<RET>();
}
template <typename LAMBDA> inline void kr::V8Object::setFunction(Text16 name, LAMBDA lambda)
{
	set(name, JsFunction::make(lambda));
}

inline kr::JsObject<kr::NativeObject>::JsObject(const JsArguments &args)
	:V8Object(args.getThis())
{
}
inline kr::JsObject<kr::NativeObject>::~JsObject() noexcept
{
}
inline kr::V8Object kr::JsObject<kr::NativeObject>::newInstanceRaw(JsArgumentsIn args)
{
	return classObject.newInstanceRaw(args);
}
inline kr::V8Class * kr::JsObject<kr::NativeObject>::getClass()
{
	return &classObject;
}
inline void kr::JsObject<kr::NativeObject>::finallize() noexcept
{
	kr_aligned_free(static_cast<kr::NativeObject*>(this));
}
