#pragma once


namespace v8
{
	class Data;
	class Private;
	class Value;
	class External;
	class Object;
	class ObjectTemplate;
	class Context;
	class Function;
	class FunctionTemplate;
	class Script;
	class TryCatch;
	class Array;

#ifndef CBS_EASYV8_INTERNAL
	template <typename E> class Handle
	{
		void * m_data;
		Handle() = delete;
	};
	template<class T> class NonCopyablePersistentTraits;
	template<class T, class M = NonCopyablePersistentTraits<T> > class Persistent
	{
		void * m_data;
		Persistent() = delete;
	};

#endif
}
namespace kr
{
	namespace _pri_
	{
		struct InternalTools;
	}

	// 자바스크립트 변수의 타입
	// JsAny 에서 getType 용도로 사용
	enum JsType
	{
		JsTypeUndefined,
		JsTypeNull,
		JsTypeExternal,
		JsTypeBoolean,
		JsTypeInteger,
		JsTypeFloat,
		JsTypeString,
		JsTypeNativeFunction,
		JsTypeV8Value,
	};

	using std::nullptr_t;
		
	class JsAny;
	class JsString;
	class JsFunction;

	template <typename T> class V8Handle;
	typedef V8Handle<v8::Data> V8Data;
	typedef V8Handle<v8::Private> V8Private;
	typedef V8Handle<v8::Value> V8Value;
	typedef V8Handle<v8::Object> V8Object;
	typedef V8Handle<v8::Array> V8Array;
	typedef V8Handle<v8::Function> V8Function;
	typedef V8Handle<v8::FunctionTemplate> V8Class;

	class JsException;
	class JsContext;
	class JsArguments;
	class JsAccessor;
	class JsIndexAccessor;
	class JsFilter;
	class NativeObject;
	class JsFieldMaker;

	template <typename T>
	class JsField;
	template <typename T>
	class JsClass;
	typedef JsAny(*JSCallback)(const JsArguments &);
	typedef RefArray<JsAny> JsArgumentsIn;

	enum undefined_t
	{
		undefined
	};

	template <typename FUNC>
	class JsNativeFunction;

}
