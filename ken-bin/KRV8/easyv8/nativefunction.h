#pragma once

#include "external.h"

namespace kr
{
	// 네이티브 함수
	// 자바스크립트로 네이티브 함수를 넣기 위해 사용
	class JsFunction
	{
		friend _pri_::InternalTools;
		friend V8Class;
		friend JsAny;
	private:
		class Data : public JsExternalData
		{
		public:
			CBS_EASYV8_DLLEXPORT V8Function CT_FASTCALL getV8Value() noexcept;
			virtual JsAny call(const JsArguments & args) = 0;
		};

		template <typename LAMBDA> class LambdaWrap : public Data
		{
		private:
			const LAMBDA m_lambda;

		public:
			LambdaWrap(LAMBDA lambda) noexcept
				:m_lambda(lambda)
			{
			}
			virtual void remove() noexcept override
			{
				delete this;
			}
			virtual JsAny call(const JsArguments & args) override
			{
				return m_lambda(args);
			}

		};

		Data * m_func;
	public:

		inline JsFunction() noexcept
		{
			m_func = nullptr;
		}
		inline bool isEmpty() const noexcept
		{
			return m_func == nullptr;
		}
		template <typename LAMBDA>
		inline JsFunction(LAMBDA func) noexcept
		{
			m_func = _new LambdaWrap<LAMBDA>(func);
			m_func->AddRef();
		}
		template <typename FUNC>
		inline JsFunction(const JsNativeFunction<FUNC> &func) noexcept
		{
			m_func = func.m_func;
			m_func->AddRef();
		}
		template <typename FUNC>
		inline JsFunction(JsNativeFunction<FUNC> &&func) noexcept
		{
			m_func = func.m_func;
			func.m_func = nullptr;
		}
		inline ~JsFunction() noexcept
		{
			if (m_func == nullptr) return;
			m_func->Release();
		}
		inline JsFunction(const JsFunction &func) noexcept
		{
			m_func = func.m_func;
			m_func->AddRef();
		}
		inline JsFunction(JsFunction &&func) noexcept
		{
			m_func = func.m_func;
			func.m_func = nullptr;
		}

		inline operator V8Function() noexcept
		{
			if (m_func == nullptr)
				return nullptr;
			return m_func->getV8Value();
		}

		inline JsAny call(const JsArguments & args) const
		{
			return m_func->call(args);
		}
		inline JsFunction& operator =(const JsFunction& _copy)
		{
			this->~JsFunction();
			new(this) JsFunction(_copy);
			return *this;
		}
		inline JsFunction& operator =(JsFunction&& _copy)
		{
			this->~JsFunction();
			new(this) JsFunction(move(_copy));
			return *this;
		}
		template <typename FUNC>
		inline JsFunction& operator =(const JsNativeFunction<FUNC>& _copy)
		{
			this->~JsFunction();
			new(this) JsFunction(_copy);
			return *this;
		}
		template <typename FUNC>
		inline JsFunction& operator =(JsNativeFunction<FUNC>&& _copy)
		{
			this->~JsFunction();
			new(this) JsFunction(move(_copy));
			return *this;
		}
		inline bool operator ==(const JsFunction & o) const
		{
			return m_func == o.m_func;
		}
		inline bool operator !=(const JsFunction & o) const
		{
			return m_func != o.m_func;
		}

		template <typename FUNC>
		friend class JsNativeFunction;
		template <typename LAMBDA>
		static typename JsMeta<LAMBDA>::ccfunc make(LAMBDA func)
		{
			return typename JsMeta<LAMBDA>::ccfunc(func);
		}

	};

	template <typename RET, typename ... ARGS> 
	class JsNativeFunction<RET(ARGS ...)> :public JsFunction
	{
	public:

		// 일반 함수를 자바스크립트 함수로 만들어냅니다.
		// 제한된 타입만 사용할 수 있습니다.
		// 사용 가능한 타입: bool, int, float, double, std::wstring
		template <typename LAMBDA> JsNativeFunction(LAMBDA func)
		{
			auto lambda = [func](const JsArguments & args)->JsAny
			{
				return JsMeta<LAMBDA>::call(func, args);
			};
			m_func = _new LambdaWrap<decltype(lambda)>(move(lambda));
			m_func->AddRef();
		}

		// 객체의 함수를 호출합니다.
		RET call(V8Object _this, const ARGS & ... args) const
		{
			JsArguments jsargs(_this, sizeof ... (args));

			size_t i = sizeof ... (args)-1;
			unwrap((jsargs[i--] = args) ...);

			return JsFunction::call(jsargs).get<RET>();
		}
	};
}