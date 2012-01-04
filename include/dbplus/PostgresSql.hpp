/*
  DBplus Copyright (C) 2012 Rafael Dantas Justo

  This file is part of DBplus.

  DBplus is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  DBplus is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with DBplus.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __DB_PLUS_POSTGRES_SQL_HPP__
#define __DB_PLUS_POSTGRES_SQL_HPP__

extern "C" {
#include <postgresql/libpq-fe.h>
}

#include <dbplus/Dbplus.hpp>

#include "RelationalDatabase.hpp"

DBPLUS_NS_BEGIN

/*! \class PostgresSql
 *  \brief PostgreSQL client interface.
 *
 * Connect to PostgreSQL database.
 */
class PostgresSql : public RelationalDatabase
{
public:
	/*! Default contructor. Nothing special here.
	 */
	PostgresSql();

	/*! Disconnect from the database.
	 */
	~PostgresSql();

	/*! Connect to the database and keeps connected until the disconnect
	 * method is called.
	 *
	 * @param database Name of the database
	 * @param user Username used for connection
	 * @param password Username's password
	 * @param server Hostname or IP address of the database
	 * @param port Database server's port. By default is 5432
	 * @see disconnect
	 * @throw DatabaseException on error
	 */
	void connect(const string &database, 
	             const string &user, 
	             const string &password, 
	             const string &server, 
	             const unsigned int port = 5432);

	/*! Disconnect from the database
	 */
	void disconnect();

	/*! Sets transaction mode. Possible values are defined in
	 * RelationalDatabase::TransactionMode::Value.
	 *
	 * @param mode Transaction mode
	 * @throw DatabaseException on error
	 */
	void setTransactionMode(const TransactionMode::Value mode);

	/*! Gets transaction mode. Possible values are defined in
	 * RelationalDatabase::TransactionMode::Value.
	 *
	 * @return Transaction mode
	 */
	TransactionMode::Value getTransactionMode() const;

	/*! In transaction mode MANUAL_COMMIT, this method is responsable
	 * for persisting every query made since the last call of the
	 * methods commit or rollback.
	 *
	 * @throw DatabaseException on error
	 * @see rollback
	 */
	void commit();

	/*! In transaction mode MANUAL_COMMIT, this method is responsable
	 * for rolling back all changes made since the last call of the
	 * methods commit or rollback.
	 *
	 * @throw DatabaseException on error
	 * @see commit
	 */
	void rollback();

	/*! To avoid SQL injection over database queries, this method
	 * removes all dangerous characters.
	 *
	 * @param value String to be escaped
	 * @return Value escaped
	 */
	string escape(const string &value);

	/*! Execute a SQL query.
	 *
	 * @param query SQL query
	 * @param resultMode Define where the result is going to be stored
	 * @return Result object with all the dataset
	 * @throw DatabaseException on error
	 */
	std::shared_ptr<Result>	
	execute(const string &query, 
	        const ResultMode::Value resultMode = ResultMode::STORE_RESULT);

	/*! Returns the number of rows effected by the last query.
	 *
	 * @return Number of rows effected
	 */
	unsigned long long affectedRows();

	/*! Returns the last inserted id due to SQL insert commands.
	 *
	 * @return Last inserted id
	 * @todo Not yet done
	 */
	unsigned long long lastInsertedId();

private:
	static void noticeReceiver(void *arg, const PGresult *result);

	PGconn *_postgres;
	TransactionMode::Value _transactionMode;
	unsigned int _affectedRows;
};

DBPLUS_NS_END

#endif // __DB_PLUS_POSTGRES_SQL_HPP__
