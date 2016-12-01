#pragma once

namespace kr
{
	namespace _pri_
	{
		template <typename T> class FinallyClass
		{
		public:
			inline FinallyClass(T lambda) noexcept;
			inline ~FinallyClass();

		private:
			T m_lambda;
		};
		template <typename T> 
		inline FinallyClass<T>::FinallyClass(T lambda) noexcept
			:m_lambda(lambda)
		{
		}
		template <typename T> 
		inline FinallyClass<T>::~FinallyClass()
		{
			m_lambda();
		}

		class StaticCode
		{
		public:
			template <typename LAMBDA>
			inline StaticCode(LAMBDA lambda)
			{
				lambda();
			}
		};
	}
}

#define staticCode	\
	static kr::_pri_::StaticCode UNIQUE(STATIC_LAMBDA) = []
#define finally(lambda)		\
	auto UNIQUE(FINALLY_LAMBDA) = lambda; \
	kr::_pri_::FinallyClass<decltype(UNIQUE(FINALLY_LAMBDA))>\
	UNIQUE(FINALLY_CLASS)(UNIQUE(FINALLY_LAMBDA));
