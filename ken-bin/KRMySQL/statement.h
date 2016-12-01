#pragma once

#include <KR3/main.h>
#include <KR3/meta/array.h>
#include <KR3/meta/types.h>
#include <KR3/meta/text.h>

#include "db.h"

namespace kr
{
	namespace sql
	{
		template <typename params_t, typename results_t> class Statement;

		template <typename T, enum_field_types type, my_bool unsign> struct MysqlTypeImpl
		{
			static void initParam(MYSQL_BIND& bind) noexcept;
			static void bindParam(MYSQL_BIND& bind, T * param) noexcept;
			static void initResult(MYSQL_BIND& bind) noexcept;
			static void bindResult(MYSQL_BIND& bind, T * param) noexcept;
			static void allocate(MYSQL_BIND& bind) noexcept;
		};
		template <typename T> struct MysqlType;
		template <> struct MysqlType<int>:MysqlTypeImpl<int, MYSQL_TYPE_LONG, false>{};;
		template <> struct MysqlType<llong> :MysqlTypeImpl<llong, MYSQL_TYPE_LONGLONG, false>{};;
		template <> struct MysqlType<long> :MysqlTypeImpl<long, MYSQL_TYPE_LONG, false>{};;
		template <> struct MysqlType<short> :MysqlTypeImpl<short, MYSQL_TYPE_SHORT, false>{};;
		template <> struct MysqlType<char>:MysqlTypeImpl<char, MYSQL_TYPE_TINY, false>{};;
		template <> struct MysqlType<uint>:MysqlTypeImpl<uint, MYSQL_TYPE_LONG, true>{};;
		template <> struct MysqlType<qword>:MysqlTypeImpl<qword, MYSQL_TYPE_LONGLONG, true>{};;
		template <> struct MysqlType<dword>:MysqlTypeImpl<dword, MYSQL_TYPE_LONG, true>{};;
		template <> struct MysqlType<word>:MysqlTypeImpl<word, MYSQL_TYPE_SHORT, true>{};;
		template <> struct MysqlType<byte>:MysqlTypeImpl<byte, MYSQL_TYPE_TINY, true>{};;
		template <> struct MysqlType<Text>
		{
			static void initParam(MYSQL_BIND& bind) noexcept;
			static void bindParam(MYSQL_BIND& bind, Text * str) noexcept;
		};;
		template <> struct MysqlType<AText>
		{
			static void initParam(MYSQL_BIND& bind) noexcept;
			static void bindParam(MYSQL_BIND& bind, AText * str) noexcept;
			static void initResult(MYSQL_BIND& bind) noexcept;
			static void bindResult(MYSQL_BIND& bind, AText * str) noexcept;
			static void allocate(MYSQL_BIND& bind) noexcept;
		};;
		template <size_t sz> struct MysqlType<BText<sz>>
		{
			static void initParam(MYSQL_BIND& bind) noexcept;
			static void bindParam(MYSQL_BIND& bind, BText<sz> * str) noexcept;
			static void initResult(MYSQL_BIND& bind) noexcept;
			static void bindResult(MYSQL_BIND& bind, BText<sz> * str) noexcept;
			static void allocate(MYSQL_BIND& bind);
		};
		template <typename C> struct MysqlType<RefArray<C>>
		{
			static_assert(sizeof(internal_component_t<C>) == 1, "Component size must be 1");
			static void initParam(MYSQL_BIND& bind) noexcept;
			static void bindParam(MYSQL_BIND& bind, RefArray<C> * str) noexcept;
		};;
		template <typename C> struct MysqlType<Array<C>>
		{
			static_assert(sizeof(internal_component_t<C>) == 1, "Component size must be 1");
			static void initParam(MYSQL_BIND& bind) noexcept;
			static void bindParam(MYSQL_BIND& bind, Array<C> * str) noexcept;
			static void initResult(MYSQL_BIND& bind) noexcept;
			static void bindResult(MYSQL_BIND& bind, Array<C> * str) noexcept;
			static void allocate(MYSQL_BIND& bind) noexcept;
		};;
		template <typename C, size_t sz> struct MysqlType<BArray<C, sz>>
		{
			static_assert(sizeof(internal_component_t<C>) == 1, "Component size must be 1");
			static void initParam(MYSQL_BIND& bind) noexcept;
			static void bindParam(MYSQL_BIND& bind, BArray<C, sz> * str) noexcept;
			static void initResult(MYSQL_BIND& bind) noexcept;
			static void bindResult(MYSQL_BIND& bind, BArray<C, sz> * str) noexcept;
			static void allocate(MYSQL_BIND& bind);
		};
		//template <> struct MysqlType<RefArray<byte>>
		//{
		//	static void initParam(MYSQL_BIND& bind) noexcept;
		//	static void bindParam(MYSQL_BIND& bind, RefArray<byte> * str) noexcept;
		//};;
		//template <> struct MysqlType<Array<byte>>
		//{
		//	static void initResult(MYSQL_BIND& bind) noexcept;
		//	static void bindResult(MYSQL_BIND& bind, Array<byte> * str) noexcept;
		//	static void allocate(MYSQL_BIND& bind) noexcept;
		//};;

		class PreparedStatementImpl
		{
		public:
			qword getInsertId() noexcept;
			qword affactedRows() noexcept;
			void execute(); // ThrowRetry, Exception
			void execute(MySQL & db); // Exception

		protected:
			PreparedStatementImpl(MySQL& sql, Text query); // Exception
			~PreparedStatementImpl() noexcept;
			
			void _bindParam(MYSQL_BIND * bind); // Exception
			void _bindResult(MYSQL_BIND * bind); // Exception
			void _storeResult(); // ThrowRetry, Exception
			void _storeResult(MySQL & db); // Exception
			void _freeResult() noexcept;
			bool _fetch(); // ThrowRetry, ThrowAllocate, Exception
			bool _fetch(MySQL & db); // ThrowAllocate, Exception
			void _fetchColumn(MYSQL_BIND * bind, uint index); // ThrowRetry, Exception

		private:
			MYSQL_STMT * m_stmt;
			Text m_query;
		};

		template <size_t index, typename ... ARGS> class StatementParam;
		template <size_t index> class StatementParam<index>:public PreparedStatementImpl
		{
		public:
			static constexpr size_t count = index;

			using PreparedStatementImpl::PreparedStatementImpl;
		};
		template <> class StatementParam<0>:public PreparedStatementImpl
		{
		public:
			static constexpr size_t count = 0;

			using PreparedStatementImpl::PreparedStatementImpl;
		};


		template <typename params, typename results = void> class Statement;

		template <typename params> class ParameterBind
		{
			template <typename params_t, typename results_t>
			friend class Statement;
		private:
			meta::array<MYSQL_BIND, params::size> m_params;

		public:
			using paramTypes = params;
			using paramPtrTypes = meta::casts<params, meta::pointer_cast>;

			ParameterBind() noexcept;
			ParameterBind(const paramPtrTypes &values) noexcept;
			ParameterBind(paramTypes * values) noexcept;
			ParameterBind& operator =(const paramPtrTypes &values) noexcept;
			ParameterBind& operator =(paramTypes * values) noexcept;
		};

		template <typename results> class ResultBind
		{
			template <typename params_t, typename results_t> 
			friend class Statement;
		private:
			meta::array<MYSQL_BIND, results::size> m_results;

		public:
			using resultTypes = results;
			using resultPtrTypes = meta::casts<results, meta::pointer_cast>;

			ResultBind() noexcept;
			ResultBind(const resultPtrTypes &values) noexcept;
			ResultBind(results * values) noexcept;
			ResultBind& operator =(const resultPtrTypes &values) noexcept;
			ResultBind& operator =(results * values) noexcept;
		};
		
		template <typename params_t, typename results_t> class Statement:public Statement<params_t, void>
		{
		private:
			ResultBind<results_t> m_resultSet;

			using PreparedStatementImpl::execute;

			bool _fetch(); // ThrowRetry, Exception
			bool _fetch(MySQL& db); // Exception
			
		public:
			class Fetcher
			{
			private:
				Statement * const m_host;
			
			public:
				Fetcher(Statement * st); // ThrowRetry, Exception
				Fetcher(MySQL & db, Statement * st); // ThrowRetry, Exception
				~Fetcher() noexcept;
				bool fetch(); // ThrowRetry, Exception
				bool fetch(MySQL& db); // Exception
			};
			
			using resultTypes = results_t;
			using typename Statement<params_t, void>::paramPtrTypes;
			using resultPtrTypes = meta::casts<results_t, meta::pointer_cast>;

			void bind(const paramPtrTypes &param, const resultPtrTypes &res) noexcept;
			void bindResults(resultTypes * values) noexcept;
			void bindResults(const resultPtrTypes &values) noexcept;
			Statement(MySQL& sql, Text text); // Exception
			void fetchColumn(uint index); // ThrowRetry, Exception
			void allocateFetch(); // ThrowRetry, Exception

			bool fetchOnce(); // ThrowRetry, Exception
			bool fetchOnce(const paramPtrTypes &param, const resultPtrTypes &res); // ThrowRetry, Exception
			template <typename LAMBDA> void fetch(const paramPtrTypes &param, const resultPtrTypes &res, LAMBDA lambda); // Exception
			template <typename LAMBDA> void fetch(const paramPtrTypes &param, LAMBDA lambda); // Exception
			template <typename LAMBDA> void fetch(LAMBDA lambda); // ThrowRetry, Exception
			template <typename LAMBDA> int fetchCount(LAMBDA lambda); // ThrowRetry, Exception

			bool fetchOnce(MySQL & db); // Exception
			bool fetchOnce(MySQL & db, const paramPtrTypes &param, const resultPtrTypes &res); // Exception
			template <typename LAMBDA> void fetch(MySQL & db, const paramPtrTypes &param, const resultPtrTypes &res, LAMBDA lambda); // Exception
			template <typename LAMBDA> void fetch(MySQL & db, const paramPtrTypes &param, LAMBDA lambda); // Exception
			template <typename LAMBDA> void fetch(MySQL & db, LAMBDA lambda); // Exception
			template <typename LAMBDA> int fetchCount(MySQL & db, LAMBDA lambda); // Exception
		};;

		template <typename params_t> class Statement<params_t, void>:public PreparedStatementImpl
		{
		private:
			ParameterBind<params_t> m_paramSet;

		public:
			using paramTypes = params_t;
			using paramPtrTypes = meta::casts<params_t, meta::pointer_cast>;

			void bind(const paramPtrTypes &values) noexcept;
			void bindParams(paramTypes * values) noexcept;
			void bindParams(const paramPtrTypes &values) noexcept;
			Statement(MySQL& sql, Text text); // Exception
			void execute(const paramPtrTypes &values) noexcept;
			void execute(MySQL & db, const paramPtrTypes &values) noexcept;
			using PreparedStatementImpl::execute;
		};;


		class Exception
		{
		public:
			Exception(MYSQL * con) noexcept;
			Exception(MYSQL_STMT* stmt) noexcept;
		};
		class ThrowRetry
		{
		};
		class ThrowAllocate
		{
		};

	
	}
}

#include "statement.inl"