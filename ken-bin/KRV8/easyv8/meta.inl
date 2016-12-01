#pragma once


template <typename CLASS, typename RET, typename ... ARGS>
template <typename FUNC>
kr::JsAny kr::JsMetaLambda<RET(CLASS::*)(ARGS ...)>::call(FUNC fn, const JsArguments & args)
{
	size_t i = sizeof ... (ARGS) - 1;
	return fn((args[i--].cast<ARGS>())...);
}

template <typename CLASS, typename ... ARGS>
template <typename FUNC>
kr::JsAny kr::JsMetaLambda<void(CLASS::*)(ARGS ...)>::call(FUNC fn, const JsArguments & args)
{
	size_t i = sizeof ... (ARGS) - 1;
	fn((args[i--].cast<ARGS>())...);
	return undefined;
}

template <typename RET, typename ... ARGS>
kr::JsAny kr::JsMeta<RET(*)(ARGS ...)>::call(RET(*fn)(ARGS ...), const JsArguments & args)
{
	size_t i = sizeof ... (ARGS) - 1;
	return fn((args[i--].cast<ARGS>())...);
}

template <typename ... ARGS>
kr::JsAny kr::JsMeta<void(*)(ARGS ...)>::call(void(*fn)(ARGS ...), const JsArguments & args)
{
	size_t i = sizeof ... (ARGS) - 1;
	fn((args[i--].cast<ARGS>())...);
	return undefined;
}

template <typename CLASS, typename RET, typename ... ARGS>
kr::JsAny kr::JsMeta<RET(CLASS::*)(ARGS ...)>::call(RET(CLASS::*fn)(ARGS ...), const JsArguments & args)
{
	size_t i = sizeof ... (ARGS) - 1;
	CLASS * _this = args.getThis().getNativeObject<CLASS>();
	if (_this == nullptr) return undefined;
	return (_this->*fn)((args[i--].cast<ARGS>())...);
}

template <typename CLASS, typename ... ARGS>
kr::JsAny kr::JsMeta<void(CLASS::*)(ARGS ...)>::call(void(CLASS::*fn)(ARGS ...), const JsArguments & args)
{
	size_t i = sizeof ... (ARGS) - 1;
	CLASS * _this = args.getThis().getNativeObject<CLASS>();
	if (_this == nullptr) return undefined;
	(_this->*fn)((args[i--].cast<ARGS>())...);
	return undefined;
}