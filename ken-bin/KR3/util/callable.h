#pragma once

namespace kr
{

	class Callable
	{
	public:
		Callable() noexcept;
		virtual ~Callable() noexcept;
		virtual void operator ()() = 0;

		template <typename LAMBDA>
		static Callable * wrap(LAMBDA lambda);
	};

	template <typename LAMBDA> 
	class CallableT:public Callable
	{
	private:
		LAMBDA m_lambda;

	public:
		inline CallableT(LAMBDA lambda)
			:m_lambda(move(lambda))
		{
		}
		~CallableT() noexcept override
		{
		}
		void operator ()() override
		{
			m_lambda();
		}
	};

	class CallablePtr
	{
	public:
		CallablePtr() noexcept = default;
		CallablePtr(Callable * _callable) noexcept;
		template <typename LAMBDA>
		CallablePtr(LAMBDA _lambda)
			:CallablePtr(Callable::wrap(move(_lambda)))
		{
		}
		operator Callable*() noexcept;
		Callable & operator *() noexcept;
		Callable * operator ->() noexcept;

	private:
		Callable * m_callable;
	};


	template <typename LAMBDA>
	Callable * Callable::wrap(LAMBDA lambda)
	{
		return _new CallableT<LAMBDA>(move(lambda));
	}

	template <typename func_t> struct LambdaTable;
	template <size_t sz, typename func_t> class Lambda;

	template <typename RET, typename ... ARGS>
	struct LambdaTable<RET(ARGS ...)>
	{
		RET(*call)(const ARGS & ..., void*);
		void(*destruct)(void*);
		void(*copy)(void*, const void*);
		void(*move)(void*, const void*);

		template <typename LAMBDA>
		static const LambdaTable<RET(ARGS ...)> * get() noexcept;
	};

	template <size_t sz, typename RET, typename ... ARGS>
	class Lambda<sz, RET(ARGS ...)>
	{
	public:
		Lambda() noexcept;
		Lambda(const Lambda & _copy) noexcept;
		Lambda(Lambda && _move) noexcept;
		template <typename LAMBDA>
		Lambda(LAMBDA lambda) noexcept;
		~Lambda() noexcept;
		void clear() noexcept;
		bool empty() noexcept;
		Lambda& operator =(const Lambda & _copy) noexcept;
		Lambda& operator =(Lambda && _move) noexcept;
		RET operator ()(const ARGS &... args);

	private:
		const LambdaTable<RET(ARGS ...)>* _getEmptyTable() noexcept;

		const LambdaTable<RET(ARGS ...)> * m_table;
		byte m_lambda[sz];
	};

	template <typename RET, typename ... ARGS>
	template <typename LAMBDA>
	const LambdaTable<RET(ARGS ...)> * LambdaTable<RET(ARGS ...)>::get() noexcept
	{
		static const LambdaTable<RET(ARGS ...)> vtable = {
			([](const ARGS &... args, void * lambda) noexcept->RET { return (*(LAMBDA*)lambda)(args ...); }),
			([](void * lambda) noexcept { ((LAMBDA*)lambda)->~LAMBDA(); }),
			([](void * dest, const void * src) noexcept {
				new((LAMBDA*)dest) LAMBDA(*(LAMBDA*)src);
			}),
			([](void * dest, const void * src) noexcept {
				new((LAMBDA*)dest) LAMBDA(kr::move(*(LAMBDA*)src));
			})
		};
		return &vtable;
	}

	template <size_t sz, typename RET, typename ... ARGS>
	Lambda<sz, RET(ARGS ...)>::Lambda() noexcept
	{
		m_table = _getEmptyTable();
	}
	template <size_t sz, typename RET, typename ... ARGS>
	Lambda<sz, RET(ARGS ...)>::Lambda(const Lambda & _copy) noexcept
	{
		m_table = _copy.m_table;
		m_table->copy(m_lambda, _copy.m_lambda);
	}
	template <size_t sz, typename RET, typename ... ARGS>
	Lambda<sz, RET(ARGS ...)>::Lambda(Lambda && _move) noexcept
	{
		m_table = _move.m_table;
		m_table->move(m_lambda, _move.m_lambda);
	}
	template <size_t sz, typename RET, typename ... ARGS>
	template <typename LAMBDA>
	Lambda<sz, RET(ARGS ...)>::Lambda(LAMBDA lambda) noexcept
	{
		static_assert(sizeof(LAMBDA) <= sizeof(m_lambda), "Lambda size unmatch");
		m_table = LambdaTable<RET(ARGS ...)>::template get<LAMBDA>();
		new((LAMBDA*)m_lambda) LAMBDA(lambda);
	}
	template <size_t sz, typename RET, typename ... ARGS>
	Lambda<sz, RET(ARGS ...)>::~Lambda() noexcept
	{
		m_table->destruct(m_lambda);
	}
	template <size_t sz, typename RET, typename ... ARGS>
	void Lambda<sz, RET(ARGS ...)>::clear() noexcept
	{
		m_table->destruct(m_lambda);
		m_table = _getEmptyTable();
	}
	template <size_t sz, typename RET, typename ... ARGS>
	bool Lambda<sz, RET(ARGS ...)>::empty() noexcept
	{
		return m_table == _getEmptyTable();
	}
	template <size_t sz, typename RET, typename ... ARGS>
	Lambda<sz, RET(ARGS ...)>& Lambda<sz, RET(ARGS ...)>::operator =(const Lambda & _copy) noexcept
	{
		m_table->destruct(m_lambda);
		m_table = _copy.m_table;
		m_table->copy(m_lambda, _copy.m_lambda);
		return *this;
	}
	template <size_t sz, typename RET, typename ... ARGS>
	Lambda<sz, RET(ARGS ...)>& Lambda<sz, RET(ARGS ...)>::operator =(Lambda && _move) noexcept
	{
		m_table->destruct(m_lambda);
		m_table = _move.m_table;
		m_table->move(m_lambda, _move.m_lambda);
		return *this;
	}
	template <size_t sz, typename RET, typename ... ARGS>
	RET Lambda<sz, RET(ARGS ...)>::operator ()(const ARGS & ... args)
	{
		return m_table->call(args ..., m_lambda);
	}
	template <size_t sz, typename RET, typename ... ARGS>
	const LambdaTable<RET(ARGS ...)>* Lambda<sz, RET(ARGS ...)>::_getEmptyTable() noexcept
	{
		static const LambdaTable<RET(ARGS ...)> emptyTable = {
			([](const ARGS & ..., void *) noexcept->RET { return RET(); }),
			([](void *) noexcept {}),
			([](void * dest, const void * src) noexcept {}),
			([](void * dest, const void * src) noexcept {})
		};
		return &emptyTable;
	}

	using LambdaP = Lambda<sizeof(void*), void()>;

}