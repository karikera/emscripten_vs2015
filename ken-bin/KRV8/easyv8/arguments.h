#pragma once

namespace kr
{

	// 자바스크립트 함수의 파라미터 목록
	class JsArguments
	{
	private:
		V8Function m_callee;
		V8Object m_this;
		Array<JsAny> m_arguments;

	public:
		inline JsArguments(const V8Function &_callee, const V8Object &_this, JsArgumentsIn args)
			:m_callee(_callee), m_this(_this)
		{
			size_t sz = args.size();
			m_arguments.resize(sz);
			for (size_t i = 0;i < sz;i++)
			{
				m_arguments[i] = args[i];
			}
		}
		inline JsArguments(const V8Function &_callee, const V8Object &_this, size_t sz)
			:m_callee(_callee), m_this(_this)
		{
			m_arguments.resize(sz);
		}
		inline JsArguments(JsArguments&& _move)
		{
			m_arguments = move(_move.m_arguments);
			m_callee = move(_move.m_callee);
			m_this = move(_move.m_this);
		}
		inline ~JsArguments()
		{
		}

		inline JsArguments& operator =(JsArguments&& _move)
		{
			this->~JsArguments();
			new(this) JsArguments(move(_move));
			return *this;
		}
		inline JsAny& operator [](size_t i)
		{
			_assert(i <= m_arguments.size());
			return m_arguments[i];
		}
		inline const JsAny& operator [](size_t i) const
		{
			static const JsAny undefined;
			if (i >= m_arguments.size()) return undefined;
			return m_arguments[i];
		}
		inline size_t size() const
		{
			return m_arguments.size();
		}

		inline const V8Function& getCallee() const
		{
			return m_callee;
		}
		inline const V8Object& getThis() const
		{
			return m_this;
		}
		inline JsAny * begin()
		{
			return m_arguments.begin();
		}
		inline JsAny * end()
		{
			return m_arguments.end();
		}
		inline const JsAny * begin() const
		{
			return m_arguments.begin();
		}
		inline const JsAny * end() const
		{
			return m_arguments.end();
		}
	};

}