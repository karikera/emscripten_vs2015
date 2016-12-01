#pragma once

#include <KR3/main.h>
#include "mysql_ref.h"

namespace kr
{

	namespace sql
	{
		class MySQLServer
		{
		public:
			MySQLServer() noexcept;
			~MySQLServer() noexcept;
		};
		class MySQL
		{
		public:
			/*
			id - keep reference
			password - keep reference
			db - keep reference
			*/
			MySQL(const char * id, const char * password, const char * db, const char * charset = nullptr) noexcept;
			~MySQL() noexcept;
			MYSQL* get() noexcept;
			void ready() noexcept;
			void commit() noexcept;
			void rollback() noexcept;
			void reconnect(); // Exception
			bool setCharset(Text charset) noexcept;
			void query(Text query); // kr::ThrowRetry, kr::sql::Exception
			void query(MySQL & db, Text qr); // kr::sql::Exception
			void storeResult(); // kr::ThrowRetry, kr::sql::Exception
			void storeResult(MySQL & db); // kr::sql::Exception
			qword affectedRows() noexcept;
			qword getInsertId() noexcept;
			
			template <typename LAMBDA>
			inline bool connection(LAMBDA &lambda); // sql::Exception
			template <typename LAMBDA>
			inline bool transection(LAMBDA &lambda); // sql::Exception

		private:
			MYSQL * m_conn;
			const char * const m_id;
			const char * const m_password;
			const char * const m_db;
			const char * const m_charset;

			template <typename LAMBDA>
			inline bool _transectionSEH(LAMBDA &lambda); // sql::Exception
			void _connect(); // kr::sql::Exception
			void _clearResult(); // kr::ThrowRetry, kr::sql::Exception
		};

	}
}
		
template <typename LAMBDA>
inline bool kr::sql::MySQL::connection(LAMBDA &lambda) // sql::Exception
{
	for (;;)
	{
		try
		{
			return meta::returnBool(lambda)();
		}
		catch (ThrowRetry&)
		{
			reconnect();
		}
		catch (Exception&e)
		{
			throw e;
		}
	}
}
template <typename LAMBDA>
inline bool kr::sql::MySQL::transection(LAMBDA &lambda) // sql::Exception
{
	for (;;)
	{
		try
		{
			ready();
			bool res = _transectionSEH(lambda);
			commit();
			return res;
		}
		catch (ThrowRetry&)
		{
			reconnect();
		}
		catch (Exception&e)
		{
			rollback();
			throw e;
		}
	}
}
template <typename LAMBDA>
inline bool kr::sql::MySQL::_transectionSEH(LAMBDA &lambda) // sql::Exception
{
	__try
	{
		return meta::returnBool(lambda)();
	}
	__except(1)
	{
		rollback();
		assert(!"Transection Error");
		throw;
	}
}
