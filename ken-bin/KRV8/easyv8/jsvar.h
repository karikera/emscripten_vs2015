#pragma once

#include <KR3/main.h>
#include <type_traits>

namespace kr
{
	template <JsType type> struct GetTypeFromJsType;
	template <> struct GetTypeFromJsType<JsTypeBoolean> { typedef bool type; };
	template <> struct GetTypeFromJsType<JsTypeExternal> { typedef void* type; };
	template <> struct GetTypeFromJsType<JsTypeFloat> { typedef double type; };
	template <> struct GetTypeFromJsType<JsTypeInteger> { typedef int type; };
	template <> struct GetTypeFromJsType<JsTypeNull> { typedef nullptr_t type; };
	template <> struct GetTypeFromJsType<JsTypeUndefined> { typedef undefined_t type; };
	template <> struct GetTypeFromJsType<JsTypeString> { typedef JsString type; };
	template <> struct GetTypeFromJsType<JsTypeV8Value> { typedef V8Value type; };
	template <> struct GetTypeFromJsType<JsTypeNativeFunction> { typedef JsFunction type; };
	template <typename T> struct GetJsTypeFromType;
	template <> struct GetJsTypeFromType<bool> { static const JsType value = JsTypeBoolean; };
	template <> struct GetJsTypeFromType<void*> { static const JsType value = JsTypeExternal; };
	template <> struct GetJsTypeFromType<double> { static const JsType value = JsTypeFloat; };
	template <> struct GetJsTypeFromType<int> { static const JsType value = JsTypeInteger; };
	template <> struct GetJsTypeFromType<nullptr_t> { static const JsType value = JsTypeNull; };
	template <> struct GetJsTypeFromType<undefined_t> { static const JsType value = JsTypeUndefined; };
	template <> struct GetJsTypeFromType<JsString> { static const JsType value = JsTypeString; };
	template <> struct GetJsTypeFromType<V8Value> { static const JsType value = JsTypeV8Value; };
	template <> struct GetJsTypeFromType<JsFunction> { static const JsType value = JsTypeNativeFunction; };

	//////////////////////////
	/// MACRO BEGIN
#define EZV8_TYPE_CONSTLIZE_SINGLE(typeValue, fn) \
case typeValue: { typedef GetTypeFromJsType<typeValue>::type type; fn; } break;

#define EZV8_TYPE_CONSTLIZE( ... ) {\
switch(getType())\
{\
EZV8_TYPE_CONSTLIZE_SINGLE(JsTypeBoolean, __VA_ARGS__)\
EZV8_TYPE_CONSTLIZE_SINGLE(JsTypeExternal, __VA_ARGS__)\
EZV8_TYPE_CONSTLIZE_SINGLE(JsTypeFloat, __VA_ARGS__)\
EZV8_TYPE_CONSTLIZE_SINGLE(JsTypeInteger, __VA_ARGS__)\
EZV8_TYPE_CONSTLIZE_SINGLE(JsTypeNull, __VA_ARGS__)\
EZV8_TYPE_CONSTLIZE_SINGLE(JsTypeString, __VA_ARGS__)\
EZV8_TYPE_CONSTLIZE_SINGLE(JsTypeUndefined, __VA_ARGS__)\
EZV8_TYPE_CONSTLIZE_SINGLE(JsTypeV8Value, __VA_ARGS__)\
EZV8_TYPE_CONSTLIZE_SINGLE(JsTypeNativeFunction, __VA_ARGS__)\
default: debug();\
}\
}
	/// MACRO END
	//////////////////////////

	// 자바스크립트 변수를 읽기 위한 클래스
	class JsAny
	{
		friend _pri_::InternalTools;
	private:
		JsType	m_type;
		long long m_data;

		template <typename T> T* _as();
		template <typename T> const T* _as() const;

		template <typename T>
		inline void _ctor(const T & value);
		template <typename T>
		inline void _ctor(T && value);
		template <>
		inline void _ctor<JsAny>(const JsAny & value);
		template <>
		inline void _ctor<JsAny>(JsAny && value);

	public:
		inline JsAny();
		template <typename T> inline JsAny(const T & value);
		template <typename T> inline JsAny(T & value);
		template <typename T> inline JsAny(T && value);
		inline JsAny(const JsAny& v);
		inline JsAny(JsAny&& v);
		inline ~JsAny();

		// 변수의 타입을 가져온다.
		inline JsType getType() const;

		// 값을 가져온다. bool, int, float, double, AText, AText16, void* 만 지원된다.
		// 타입이 다를경우 변환을 하여 가져오게 된다.
		template <typename T> T cast() const;

		template <>
		inline void cast<void>() const;

		// 내부에서 타입 확인 없이, 강제로 캐스트 하여 가져온다.
		template <typename T> const T& as() const;

		// 내부에서 타입 확인 없이, 강제로 캐스트 하여 가져온다.
		template <typename T> T& as();


		inline JsAny& operator =(const JsAny& _copy);
		inline JsAny& operator =(JsAny&& _move);

		inline bool operator ==(const JsAny & args) const;
		inline bool operator !=(const JsAny & args) const;
	};

	class JsLambda
	{
	public:
		class Data
		{
		public:
			virtual void call() = 0;
			virtual void remove() = 0;
		private:

		};

	private:

		template <typename LAMBDA>
		class DataImpl : public Data
		{
		private:
			LAMBDA m_lambda;

		public:
			DataImpl(LAMBDA lambda)
				:m_lambda(move(lambda))
			{
			}
			virtual void call() override
			{
				m_lambda();
			}
			virtual void remove() override
			{
				delete this;
			}
		};
		Data * m_data;

	public:
		inline JsLambda()
		{
			m_data = nullptr;
		}
		template <typename LAMBDA>
		inline JsLambda(LAMBDA lambda)
		{
			m_data = _new DataImpl<LAMBDA>(move(lambda));
		}
		inline JsLambda(JsLambda && c)
		{
			m_data = c.m_data;
			c.m_data = nullptr;
		}
		inline ~JsLambda()
		{
			if (m_data != nullptr) m_data->remove();
		}
		inline Data * detach()
		{
			Data * data = m_data;
			m_data = nullptr;
			return data;
		}
		inline void operator ()() const
		{
			m_data->call();
		}
		inline JsLambda& operator =(JsLambda && c)
		{
			this->~JsLambda();
			m_data = c.m_data;
			c.m_data = nullptr;
			return *this;
		}
	};
	
	// 스트링
	class JsString:public Bufferable<JsString, BufferInfo<char16, true, true>>
	{
		friend JsAny;
	public:
		inline JsString();
		inline JsString(nullptr_t);
		inline JsString(const JsString & _copy);
		inline JsString(JsString && _move);
		inline JsString(Text str, Charset cs = Charset::Default) noexcept;
		inline JsString(Text16 str);
		inline JsString(int n);
		inline JsString(double n);
		inline JsString(bool b);
		inline JsString(undefined_t);
		inline JsString(void *);
		CBS_EASYV8_DLLEXPORT JsString(const V8Handle<v8::Value> & obj);
		inline JsString(const JsFunction & obj);
		inline ~JsString();
		inline char16 * begin() noexcept;
		inline char16 * end() noexcept;
		inline const char16 * begin() const noexcept;
		inline const char16 * end() const noexcept;
		inline void alloc(size_t sz) noexcept;
		inline void free() noexcept;
		inline size_t size() const noexcept;
		inline JsString& operator =(const JsString& _copy);
		inline JsString& operator =(JsString&& _move);
		inline pcstr16 c_str() const;
		inline size_t length() const;
		inline operator Text16() const noexcept;
		inline explicit operator AText() const noexcept;
		inline explicit operator AText16() const noexcept;

		inline bool operator ==(nullptr_t) const;
		inline bool operator !=(nullptr_t) const;
		inline bool operator ==(const JsString & args) const;
		inline bool operator !=(const JsString & args) const;
		
	private:
		inline void rawFree() noexcept;
		inline void rawAlloc(size_t sz) noexcept;

		char16 * m_data;
	};

	template <typename T> T& js_cast()
	{
		return T;
	}
}
