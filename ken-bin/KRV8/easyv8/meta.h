#pragma once

namespace kr
{
	// 함수를 분해해서, JsAny를 파라미터로 받는 함수로 랩핑 시키는 부분

	inline void unwrap(...) {}

	template <typename FUNC>
	struct JsMetaLambda;
	template <typename CLASS, typename RET, typename ... ARGS>
	struct JsMetaLambda<RET(CLASS::*)(ARGS ...)>
	{
		using ccfunc = JsNativeFunction<RET(ARGS ...)>;
		template <typename FUNC>
		static JsAny call(FUNC func, const JsArguments & args);
	};
	template <typename CLASS, typename ... ARGS>
	struct JsMetaLambda<void(CLASS::*)(ARGS ...)>
	{
		using ccfunc = JsNativeFunction<void(ARGS ...)>;
		template <typename FUNC>
		static JsAny call(FUNC func, const JsArguments & args);
	};
	template <typename CLASS, typename RET, typename ... ARGS>
	struct JsMetaLambda<RET(CLASS::*)(ARGS ...) const> :JsMetaLambda<RET(CLASS::*)(ARGS ...)>
	{
	};

	template <typename FUNC>
	struct JsMeta : JsMetaLambda<decltype(&FUNC::operator())>
	{
	};
	template <typename RET, typename ... ARGS>
	struct JsMeta<RET(*)(ARGS ...)>
	{
		using ccfunc = JsNativeFunction<RET(ARGS ...)>;
		static JsAny call(RET(*fn)(ARGS ...), const JsArguments & args);
	};
	template <typename ... ARGS>
	struct JsMeta<void(*)(ARGS ...)>
	{
		using ccfunc = JsNativeFunction<void(ARGS ...)>;
		static JsAny call(void(*fn)(ARGS ...), const JsArguments & args);
	};
	template <typename CLASS, typename RET, typename ... ARGS>
	struct JsMeta<RET(CLASS::*)(ARGS ...)>
	{
		using ccfunc = JsNativeFunction<RET(ARGS ...)>;
		static JsAny call(RET(CLASS::*fn)(ARGS ...), const JsArguments & args);
	};
	template <typename CLASS, typename ... ARGS>
	struct JsMeta<void(CLASS::*)(ARGS ...)>
	{
		using ccfunc = JsNativeFunction<void(ARGS ...)>;
		static JsAny call(void(CLASS::*fn)(ARGS ...), const JsArguments & args);
	};
	template <typename CLASS, typename RET, typename ... ARGS>
	struct JsMeta<RET(CLASS::*)(ARGS ...) const> :JsMeta<RET(CLASS::*)(ARGS ...)>
	{
	};

}